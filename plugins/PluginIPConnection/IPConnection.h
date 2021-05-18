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
#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H
#include "IPConfig.h"
#include "IPInterface.h"
#include "IPInterfaceTCPClient.h"
// #include "IPInterfaceUDPClient.h"
#include "IPInterfaceUDPServer.h"
#include <boost/log/trivial.hpp>
#include <connection/Connection.h>
#include <plugins/DataSourcePlugin.h>
#include <queue>

class IPConnection : public IPInterface::CreateChild
{
public:
    explicit IPConnection(sgcs::plugin::DataSourcePlugin::ConnectionFabric *connectionFabric);
    virtual ~IPConnection();

    virtual void onChild(IPChild *c) override final;
    void doConnectToPort(const std::string &hostName, uint16_t port);
    void doDisconnectFromPort();

private:
    std::string m_hostName;
    uint16_t m_port          = 0;
    IPInterface *m_interface = nullptr;
    sgcs::plugin::DataSourcePlugin::ConnectionFabric *m_connectionFabric;
};

#endif // SERIALCONNECTION_H
