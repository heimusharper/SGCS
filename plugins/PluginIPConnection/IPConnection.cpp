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

IPConnection::IPConnection(sgcs::plugin::DataSourcePlugin::ConnectionFabric *connectionFabric)
: IPInterface::CreateChild()
, m_hostName(RunConfiguration::instance().get<IPConfig>()->hostName())
, m_port(RunConfiguration::instance().get<IPConfig>()->port())
, m_connectionFabric(connectionFabric)
{
    // TODO: tempolary onnly UDP client mode
    m_interface = new IPInterfaceTCPClient();
    // m_interface = new IPInterfaceUDPServer();
    m_interface->setChildsHandler(this);
    if (!m_hostName.empty() && m_port >= 1024)
    {
        // ready to connect automaticaly
        doConnectToPort(m_hostName, m_port);
    }
    m_interface->start();
}

IPConnection::~IPConnection()
{
    if (m_interface)
        delete m_interface;
    if (m_connectionFabric)
        delete m_connectionFabric;
}

void IPConnection::onChild(IPChild *c)
{
    sgcs::connection::ConnectionThread *thr = new sgcs::connection::ConnectionThread;
    m_connectionFabric->onCreate(thr, c);
}

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
        m_interface->closeConnection();
}
