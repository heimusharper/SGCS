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
, m_autoConnect(RunConfiguration::instance().get<SerialConfig>()->autoName())
, MAX_BUFFER_SIZE(50)
, MAX_READ_BYTES_SIZE(256)
{
    m_stop.store(false);
    m_isDirty.store(false);
    if (!m_portName.empty() && m_baudRate >= 9600 && !m_autoConnect)
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

void SerialConnection::processFromParent(const tools::CharMap &data)
{
    // nope
}

void SerialConnection::processFromChild(const tools::CharMap &data)
{
    m_mutex.lock();
    m_writeBuffer.push(data);
    while (m_writeBuffer.size() >= MAX_BUFFER_SIZE)
        m_writeBuffer.pop();
    m_mutex.unlock();
}

void SerialConnection::connectToPort(const std::string &portName, int baudRate)
{
    m_mutex.lock();
    m_portName    = portName;
    m_baudRate    = baudRate;
    m_autoConnect = false;
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
    char *read_buf = new char[MAX_READ_BYTES_SIZE];

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
                m_mutex.lock();
                while (!m_writeBuffer.empty())
                {
                    tools::CharMap cm = m_writeBuffer.front();
                    m_writeBuffer.pop();
                    write(serialDsc, cm.data, cm.size);
                }
                m_mutex.unlock();
                // read
                {
                    int n = read(serialDsc, &read_buf, MAX_READ_BYTES_SIZE);
                    if (n > 0)
                    {
                        tools::CharMap cm;
                        cm.data = new char[n];
                        cm.size = n;
                        memcpy(cm.data, &read_buf, n);
                        writeToChild(cm);
                    }
                }
            }
        }
        else if ((!m_portName.empty() || m_autoConnect) && m_baudRate > 0)
        {
            m_mutex.lock();
            if (m_autoConnect)
            {
                // select port
                /*if (m_portNames.empty())
                {
                    boost::filesystem::path path("/dev");
                    try
                    {
                        if (!exists(path))
                        {
                            throw std::runtime_error(path.generic_string() + " does not exist");
                        }
                        else
                        {
                            for (boost::filesystem::directory_entry &de : boost::filesystem::directory_iterator(path))
                            {
                                if (!is_symlink(de.symlink_status()))
                                {
                                    boost::filesystem::path symlink_points_at = de.path();
                                    boost::filesystem::path canonical_path = boost::filesystem::canonical(symlink_points_at, path);

                                    BOOST_LOG_TRIVIAL(debug) << "CHeck " << canonical_path.generic_string();

                                    int fd = open(canonical_path.generic_string().c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
                                    if (fd >= 0)
                                    {
                                        struct termios serinfo;
                                        // memset(&serinfo, 0, sizeof(serinfo));
                                        fcntl(fd, F_SETFL, FNDELAY);
                                        if (tcgetattr(fd, &serinfo) > 0)
                                        // if (ioctl(fd, TCSETS2, &serinfo) > 0)
                                        {
                                            m_portNames.push(canonical_path.generic_string());
                                        }
                                        close(fd);
                                    }
                                }
                            }
                        }
                    }
                    catch (const boost::filesystem::filesystem_error &ex)
                    {
                        BOOST_LOG_TRIVIAL(warning) << "Serial list check error: " << ex.what();
                    }
                    // std::sort(m_portNames.begin(), m_portNames.end());
                }
                if (!m_portNames.empty())
                {
                    m_portName = "";
                    m_portName = m_portNames.front();
                    m_portNames.pop();
                    BOOST_LOG_TRIVIAL(warning) << "Take new serial device " << m_portName;
                }
                else
                {
                    BOOST_LOG_TRIVIAL(warning) << "Serial devices not found";
                }*/
            }
            if (!m_portName.empty())
            {
                serialDsc = open(m_portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
                if (serialDsc < 0)
                    BOOST_LOG_TRIVIAL(warning)
                    << "Failed open serial device" << m_portName << " " << m_baudRate << " " << strerror(errno);
                else
                {
                    fcntl(serialDsc, F_SETFL, FNDELAY);
                    tcgetattr(serialDsc, &tty);

                    tty.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
                    tty.c_cflag |= (CREAD | CLOCAL | CS8);
                    tty.c_lflag &= ~(ICANON | ISIG);
                    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // disable software flow control
                    // tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
                    // tty.c_oflag &= ~OPOST;
                    // tty.c_oflag &= ~ONLCR;
                    tty.c_cc[VTIME] = 10;
                    tty.c_cc[VMIN]  = 10;
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

                    if (tcsetattr(serialDsc, TCSANOW, &tty) != 0)
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
            }
            m_mutex.unlock();
        }
        usleep((serialDsc < 0) ? 1000000 : 50);
    }

    if (serialDsc >= 0)
    {
        close(serialDsc);
    }
    delete[] read_buf;
}
