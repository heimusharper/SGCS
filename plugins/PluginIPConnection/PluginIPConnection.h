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
#ifndef PLUGINSERIALCONNECTION_H
#define PLUGINSERIALCONNECTION_H

#include "IPConnection.h"
#include <QObject>
#include <plugins/DataSourcePlugin.h>

class PluginIPConnection : public sgcs::plugin::DataSourcePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ru.heimusharper.SGCS.DataSourcePlugin" FILE "meta.json")
    Q_INTERFACES(sgcs::plugin::DataSourcePlugin)
public:
    explicit PluginIPConnection(QObject *parent = nullptr);
    virtual ~PluginIPConnection() = default;

    virtual QString name() const override;

    virtual sgcs::connection::Connection *instance() override;
signals:
};

#endif // PLUGINSERIALCONNECTION_H
