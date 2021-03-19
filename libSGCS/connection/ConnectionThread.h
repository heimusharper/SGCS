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
#ifndef CONNECTIONTHREAD_H
#define CONNECTIONTHREAD_H

#include "../plugins/LeafPlugin.h"
#include "../plugins/ProtocolPlugin.h"
#include "ConnectionRouter.h"

namespace sgcs
{
namespace connection
{
class ConnectionThread
{
public:
    ConnectionThread();
    ~ConnectionThread();

    void create(Connection *connection,
                const std::vector<plugin::ProtocolPlugin *> &protos,
                const std::vector<plugin::LeafPlugin *> &leafs);

private:
    ConnectionRouter *_router = nullptr;
};
}
}
#endif // CONNECTIONTHREAD_H
