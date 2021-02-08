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
ConnectionRouter::ConnectionRouter(Connection *connection, const boost::container::vector<uav::UavProtocol *> &protos)
: m_connection(connection), m_protos(protos)
{
    _stopThread.store(false);
    _thread = new std::thread(&ConnectionRouter::run, this);
}

ConnectionRouter::~ConnectionRouter()
{
    _stopThread.store(false);
    if (_thread)
    {
        _thread->join();
        delete _thread;
    }
}

void ConnectionRouter::run()
{
    while (!_stopThread.load())
    {
        // collect buffer
        if (!m_protocol && m_connection->isHasBytes())
        {
            boost::container::vector<uint8_t> bytes = m_connection->collectBytesAndClear();
            for (int i = 0; i < bytes.size(); i++)
                m_buffer.push(bytes[i]);
            if (m_buffer.size() > MAX_BUFFER_SIZE)
                while (m_buffer.size() > MAX_BUFFER_SIZE - MAX_BUFFER_SIZE / 4)
                    m_buffer.pop();
        }
        // try to create root protocol
        if (!m_protocol)
        {
            boost::container::vector<uint8_t> bytes;
            std::queue<char> tmp = m_buffer;
            while (!tmp.empty())
            {
                bytes.push_back(tmp.front());
                tmp.pop();
            }
            for (uav::UavProtocol *p : m_protos)
            {
                p->onReceived(bytes);
                if (p->isHasData())
                {
                    m_protocol = p;
                    BOOST_LOG_TRIVIAL(info) << "Ready " << m_protocol->name() << " protocol";

                    while (m_protos.empty())
                    {
                        auto obj = m_protos.back();
                        if (obj != m_protocol)
                            delete obj;
                        m_protos.pop_back();
                    }
                }
            }
        }
        // bridge
        if (m_protocol && m_connection && m_connection->isHasBytes())
        {
            boost::container::vector<uint8_t> bytes = m_connection->collectBytesAndClear();
            BOOST_LOG_TRIVIAL(info) << "READ DATA SIZE " << bytes.size();
            if (!bytes.empty())
            {
                m_protocol->onReceived(bytes);
            }
        }
        usleep(10000);
    }
}
}
}
