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
#ifndef CONNECTIONROUTER_H
#define CONNECTIONROUTER_H

#include "../plugins/LeafInterface.h"
#include "../plugins/ProtocolPlugin.h"
#include "Connection.h"
#include <UAV.h>
#include <UavProtocol.h>
#include <atomic>
#include <boost/log/trivial.hpp>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace sgcs
{
namespace connection
{
class ConnectionRouter
{
public:
    ConnectionRouter(Connection *connection,
                     const std::vector<uav::UavProtocol *> &protos,
                     const std::vector<gcs::LeafInterface *> &leafs);
    ~ConnectionRouter();

private:
    void runConection();

    class UavCreateHandler : public uav::UavProtocol::UavCreateHandler
    {
    public:
        UavCreateHandler(const std::vector<gcs::LeafInterface *> &leafs) : m_leafs(leafs)
        {
        }
        virtual ~UavCreateHandler() = default;
        virtual void onCreateUav(uav::UAV *uav) override;

    private:
        std::vector<gcs::LeafInterface *> m_leafs;
    };

private:
    const std::size_t MAX_BUFFER_SIZE;
    Connection *m_connection = nullptr;
    std::vector<uav::UavProtocol *> m_protos;
    std::vector<gcs::LeafInterface *> m_leafs;

    UavCreateHandler *m_uavCreateHnadler = nullptr;

    uav::UavProtocol *m_protocol = nullptr;

    std::queue<char> m_buffer;

    std::thread *m_connectionsThread = nullptr;
    std::atomic_bool m_stopThread {false};
};
}
}
#endif // CONNECTIONROUTER_H
