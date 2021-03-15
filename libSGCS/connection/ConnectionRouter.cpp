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

#include "ConnectionRouter.h"

namespace sgcs
{
namespace connection
{
ConnectionRouter::ConnectionRouter(Connection *connection,
                                   const std::vector<uav::UavProtocol *> &protos,
                                   const std::vector<gcs::LeafInterface *> &leafs)
: MAX_BUFFER_SIZE(2048), m_connection(connection), m_protos(protos), m_leafs(leafs)
{
    m_stopThread.store(false);
    m_connectionsThread = new std::thread(&ConnectionRouter::runConection, this);
    m_uavCreateHnadler  = new UavCreateHandler(m_leafs);
}

ConnectionRouter::~ConnectionRouter()
{
    m_stopThread.store(false);
    if (m_connectionsThread)
    {
        if (m_connectionsThread->joinable())
            m_connectionsThread->join();
        delete m_connectionsThread;
    }
}

void ConnectionRouter::runConection()
{
    while (!m_stopThread.load())
    {
        // collect buffer
        if (!m_protocol && m_connection->isHasBytes())
        {
            std::vector<uint8_t> bytes = m_connection->collectBytesAndClear();
            for (std::size_t i = 0; i < bytes.size(); i++)
                m_buffer.push(bytes[i]);
            if (m_buffer.size() > MAX_BUFFER_SIZE)
                while (m_buffer.size() > MAX_BUFFER_SIZE - MAX_BUFFER_SIZE / 4)
                    m_buffer.pop();
        }
        // try to create root protocol
        if (!m_protocol && !m_buffer.empty())
        {
            std::vector<uint8_t> bytes;
            std::queue<char> tmp = m_buffer;
            while (!tmp.empty())
            {
                bytes.push_back(tmp.front());
                tmp.pop();
            }
            auto iter = std::find_if(m_protos.begin(), m_protos.end(), [bytes](uav::UavProtocol *proto) {
                proto->onReceived(bytes);
                return proto->isReadyMessages();
            });

            if (iter != m_protos.end())
            {
                m_protocol = *iter;
                BOOST_LOG_TRIVIAL(info) << "Ready " << m_protocol->name() << " protocol";
                while (m_protos.empty())
                {
                    auto obj = m_protos.back();
                    if (obj != m_protocol)
                        delete obj;
                    m_protos.pop_back();
                }
                m_protocol->addUavCreateHandler(m_uavCreateHnadler);
            }
        }

        if (!m_protocol)
        {
            // send HELLO
            for (const uav::UavProtocol *proto : m_protos)
            {
                std::vector<uint8_t> hello = proto->hello();
                if (!hello.empty())
                    m_connection->onTransmit(hello);
            }
        }
        // bridge
        if (m_protocol && m_connection && m_connection->isHasBytes())
        {
            std::vector<uint8_t> bytes = m_connection->collectBytesAndClear();
            // BOOST_LOG_TRIVIAL(info) << "READ DATA SIZE " << bytes.size();
            if (!bytes.empty())
            {
                m_protocol->onReceived(bytes);
            }
        }
        usleep(100000);
    }
}

void ConnectionRouter::UavCreateHandler::onCreateUav(uav::UAV *uav)
{
    if (uav)
        for (gcs::LeafInterface *l : m_leafs)
            l->setUAV(uav);
}

}
}
