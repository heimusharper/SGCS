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
#include "ConnectionThread.h"
namespace sgcs
{
namespace connection
{
ConnectionThread::ConnectionThread()
{
}

ConnectionThread::~ConnectionThread()
{
    if (_router)
        delete _router;
}

void ConnectionThread::create(Connection *connection,
                              const std::vector<plugin::ProtocolPlugin *> &protos,
                              const std::vector<plugin::LeafPlugin *> &leafs)
{
    std::vector<sgcs::connection::UavProtocol *> protosImpl;
    for (auto x : protos)
    {
        auto inst = x->instance();
        if (inst)
            protosImpl.push_back(inst);
    }
    std::vector<gcs::LeafInterface *> leafsImpl;
    for (auto x : leafs)
    {
        auto leaf = x->leaf();
        if (leaf)
            leafsImpl.push_back(leaf);
    }
    _router = new ConnectionRouter(connection, protosImpl, leafsImpl);
}
}
}
