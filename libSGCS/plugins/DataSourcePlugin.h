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
#ifndef DATASOURCEPLUGIN_H
#define DATASOURCEPLUGIN_H

#include "../connection/Connection.h"
#include "../connection/ConnectionThread.h"
#include "PluginInterface.h"

namespace sgcs
{
namespace plugin
{
class DataSourcePlugin : public PluginInterface
{
public:
    DataSourcePlugin();
    virtual ~DataSourcePlugin() = default;

    virtual sgcs::connection::Connection *instance() = 0;

    class ConnectionFabric
    {
    public:
        virtual ~ConnectionFabric() = default;
        virtual void onCreate(sgcs::connection::ConnectionThread *thr, sgcs::connection::Connection *instance) = 0;
    };

    virtual void startConnectionFabric(ConnectionFabric *)
    {
    }

    bool hasConnectionFabric() const;

protected:
    bool m_hasConnectionFabric;
};
}
}

#endif // DATASOURCEPLUGIN_H
