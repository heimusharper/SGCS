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
                                   const std::vector<UavProtocol *> &protos,
                                   const std::vector<gcs::LeafInterface *> &leafs)
: MAX_BUFFER_SIZE(2048), m_connection(connection), m_protos(protos), m_leafs(leafs)
{
    assert(m_connection != nullptr);
    m_connection->addChild(this);

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

void ConnectionRouter::processFromChild(const tools::CharMap &data)
{
}

void ConnectionRouter::processFromParent(const tools::CharMap &data)
{
    m_bufferMutex.lock();
    m_buffer.push_back(data);
    m_bufferMutex.unlock();
}

void ConnectionRouter::runConection()
{
    while (!m_stopThread.load())
    {
        // try to create root protocol
        if (!m_protocol)
        {
            while (!m_buffer.empty())
            {
                m_bufferMutex.lock();
                auto data = m_buffer.back();
                m_buffer.pop_back();
                m_bufferMutex.unlock();
                // TODO: tools::CharMap size optimization
                if (data.size <= 0)
                    continue;
                auto iter = std::find_if(m_protos.begin(), m_protos.end(), [data](sgcs::connection::UavProtocol *proto) {
                    proto->processFromParent(data);
                    return proto->isValid();
                });
                if (iter != m_protos.end())
                {
                    m_connection->removeChild(this); // end protocol creation
                    m_buffer.clear();
                    m_protocol = *iter;
                    BOOST_LOG_TRIVIAL(info) << "Ready " << m_protocol->name() << " protocol";
                    while (!m_protos.empty())
                    {
                        auto obj = m_protos.back();
                        if (obj != m_protocol)
                            delete obj;
                        m_protos.pop_back();
                    }
                    m_protocol->addUavCreateHandler(m_uavCreateHnadler);
                    m_connection->addChild(m_protocol);
                    m_protocol->setParent(m_connection);

                    m_protocol->startMessaging();
                }
            }
        }

        if (!m_protocol)
        {
            // send HELLO
            for (const sgcs::connection::UavProtocol *proto : m_protos)
            {
                tools::CharMap hello = proto->hello();
                if (hello.size > 0)
                    m_connection->processFromChild(hello);
            }
        }
        if (m_protocol)
            return;
        usleep(2000000);
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
