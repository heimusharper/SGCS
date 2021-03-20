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
#include "IPConnection.h"

IPConnection::IPConnection()
: sgcs::connection::Connection()
, m_hostName(RunConfiguration::instance().get<IPConfig>()->hostName())
, m_port(RunConfiguration::instance().get<IPConfig>()->port())
{
    // TODO: tempolary onnly UDP client mode
    m_interface = new IPInterfaceTCPClient();
    m_interface->setParent(this);
    addChild(m_interface);
    // m_interface = new IPInterfaceUDPServer();
    // m_interface = new IPInterfaceUDPClient();
    if (!m_hostName.empty() && m_port >= 1024)
    {
        // ready to connect automaticaly
        doConnectToPort(m_hostName, m_port);
    }
}

IPConnection::~IPConnection()
{
    if (m_interface)
        delete m_interface;
}

void IPConnection::process(const tools::CharMap &data)
{
    writeToChild(data);
}

void IPConnection::processFromChild(const tools::CharMap &data)
{ // BOOST_LOG_TRIVIAL(debug) << "WRITE SIZE" << data.size();

    /*if (m_interface)
    {
        m_interface->process(data);
    }*/
}

/*std::vector<uint8_t> IPConnection::collectBytesAndClear()
{
    std::vector<uint8_t> bytes;
    setHasBytes(false);
    while (!m_readBuffer.empty())
    {
        bytes.push_back(m_readBuffer.front());
        m_readBuffer.pop();
    }
    return bytes;
}*/

/*bool IPConnection::isHasBytes()
{
    if (m_interface)
    {
        std::queue<uint8_t> buffer = m_interface->readBuffer();
        while (!buffer.empty())
        {
            m_readBuffer.push(buffer.front());
            buffer.pop();
        }
        if (m_readBuffer.size() > MAX_BUFFER_SIZE)
            while (m_readBuffer.size() > MAX_BUFFER_SIZE - MAX_BUFFER_SIZE / 4)
                m_readBuffer.pop();

        setHasBytes(!m_readBuffer.empty());
    }
    else
        setHasBytes(false);
    return sgcs::connection::Connection::isHasBytes();
}*/

void IPConnection::doConnectToPort(const std::string &hostName, uint16_t port)
{
    m_hostName = hostName;
    m_port     = port;
    if (m_interface)
    {
        BOOST_LOG_TRIVIAL(info) << "IP connect " << m_hostName << ":" << m_port;
        m_interface->doConnect(m_hostName, m_port);
    }
    else
        BOOST_LOG_TRIVIAL(warning) << "Interface not initialized";
}

void IPConnection::doDisconnectFromPort()
{
    if (m_interface)
        m_interface->close();
}
