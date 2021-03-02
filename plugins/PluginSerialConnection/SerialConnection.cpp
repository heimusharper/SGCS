/*
 * This file is part of the SGCS distribution (https://github.com/heimusharper/SGCS).
 * Copyright (c) 2020 Andrey Stepanov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "SerialConnection.h"

SerialConnection::SerialConnection()
: sgcs::connection::Connection()
, m_portName(RunConfiguration::instance().get<SerialConfig>()->portName())
, m_baudRate(RunConfiguration::instance().get<SerialConfig>()->baudRate())
, MAX_BUFFER_SIZE(50)
, MAX_READ_BYTES_SIZE(256)
{
    m_stop.store(false);
    m_isDirty.store(false);
    if (!m_portName.empty() && m_baudRate >= 9600)
    {
        // ready to connect automaticaly
        connectToPort(m_portName, m_baudRate);
    }
    m_thread = new std::thread(&SerialConnection::run, this);
}

SerialConnection::~SerialConnection()
{
    m_stop.store(true);
    if (m_thread)
    {
        if (m_thread->joinable())
            m_thread->join();
        delete m_thread;
    }
}

void SerialConnection::onTransmit(const boost::container::vector<uint8_t> &data)
{
    BOOST_LOG_TRIVIAL(debug) << "WRITE SIZE" << data.size();
    m_mutex.lock();
    for (int i = 0; i < data.size(); i++)
    {
        CharMap cm;
        cm.data = new char[data.size()];
        memcpy(cm.data, data.data(), data.size());
        cm.size = data.size();
        m_writeBuffer.push(cm);
    }
    while (m_writeBuffer.size() >= MAX_BUFFER_SIZE)
        m_writeBuffer.pop();
    m_mutex.unlock();
}

std::vector<uint8_t> SerialConnection::collectBytesAndClear()
{
    std::vector<uint8_t> bytes;
    setHasBytes(false);
    m_mutex.lock();
    while (!m_readBuffer.empty())
    {
        CharMap cm = m_readBuffer.front();
        m_readBuffer.pop();

        for (size_t i = 0; i < cm.size; i++)
            bytes.push_back(cm.data[i]);
    }
    m_mutex.unlock();
    return bytes;
}

void SerialConnection::connectToPort(const std::string &portName, int baudRate)
{
    m_mutex.lock();
    m_portName = portName;
    m_baudRate = baudRate;
    m_isDirty.store(true);
    m_mutex.unlock();
}

void SerialConnection::disconnectFromPort()
{
    connectToPort("", 0);
}

void SerialConnection::run()
{
    SerialDescriptor serialDsc = -1;
    struct termios tty;
    char read_buf[MAX_READ_BYTES_SIZE];

    while (!m_stop.load())
    {
        if (serialDsc >= 0)
        {
            // done
            if (m_isDirty.load())
            {
                // disconnect
                close(serialDsc);
                serialDsc = -1;
            }
            else
            {
                // write
                if (!m_writeBuffer.empty())
                {
                    m_mutex.lock();
                    CharMap cm = m_writeBuffer.front();
                    m_writeBuffer.pop();
                    m_mutex.unlock();
                    write(serialDsc, cm.data, cm.size);
                }
                // read
                {
                    int n = read(serialDsc, &read_buf, MAX_READ_BYTES_SIZE);
                    CharMap cm;
                    cm.data = new char[n];
                    memcpy(cm.data, &read_buf, n);
                    m_mutex.lock();
                    m_readBuffer.push(cm);
                    m_mutex.unlock();
                    if (cm.size > 0)
                        setHasBytes(true);
                }
            }
        }
        else if (!m_portName.empty() && m_baudRate > 0)
        {
            m_mutex.lock();
            serialDsc = open(m_portName.c_str(), O_RDWR);
            if (serialDsc < 0)
                BOOST_LOG_TRIVIAL(warning)
                << "Failed open serial device" << m_portName << " " << m_baudRate << " " << strerror(errno);
            else
            {
                tty.c_cflag &= ~PARENB;
                tty.c_cflag &= ~CSTOPB;
                tty.c_cflag |= CS8;
                tty.c_cflag &= ~CRTSCTS;
                tty.c_cflag |= CREAD | CLOCAL;
                tty.c_lflag &= ~ICANON;
                tty.c_lflag &= ~ISIG;
                tty.c_iflag &= ~(IXON | IXOFF | IXANY);
                tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
                // tty.c_oflag &= ~OPOST;
                // tty.c_oflag &= ~ONLCR;
                tty.c_cc[VTIME] = 10;
                tty.c_cc[VMIN]  = 0;
                speed_t baudrate;
                switch (m_baudRate)
                {
                    case 4800:
                        baudrate = B4800;
                        break;
                    case 9600:
                        baudrate = B9600;
                        break;
                    case 19200:
                        baudrate = B19200;
                        break;
                    case 38400:
                        baudrate = B38400;
                        break;
                    case 115200:
                        baudrate = B115200;
                        break;
                    case 57600:
                    default:
                        baudrate = B57600;
                        break;
                }
                cfsetispeed(&tty, baudrate);
                cfsetospeed(&tty, baudrate);

                if (tcgetattr(serialDsc, &tty) != 0)
                {
                    BOOST_LOG_TRIVIAL(warning)
                    << "Failed tcgetattr " << m_portName << " " << m_baudRate << " " << strerror(errno);
                    close(serialDsc);
                    serialDsc = -1;
                }
                else
                {
                    m_isDirty.store(false);
                }
            }
            m_mutex.unlock();
        }
        usleep((serialDsc < 0) ? 1000000 : 1000);
    }

    if (serialDsc >= 0)
    {
        close(serialDsc);
        serialDsc = -1;
    }
}
