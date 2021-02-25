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
#include "IPInterfaceUDPClient.h"
#include <boost/log/trivial.hpp>
#include <connection/Connection.h>
#include <queue>

class IPConnection : public sgcs::connection::Connection
{
public:
    explicit IPConnection();
    virtual ~IPConnection();
    virtual void onTransmit(const boost::container::vector<uint8_t> &data) override final;
    virtual std::vector<uint8_t> collectBytesAndClear() override final;

    virtual bool isHasBytes() override final;
    void doConnectToPort(const std::string &hostName, uint16_t port);
    void doDisconnectFromPort();

private:
    std::string m_hostName;
    uint16_t m_port           = 0;
    const int MAX_BUFFER_SIZE = 1024 * 10;

    std::queue<uint8_t> m_writeBuffer;
    std::queue<uint8_t> m_readBuffer;

    IPInterface *m_interface = nullptr;
};

#endif // SERIALCONNECTION_H
