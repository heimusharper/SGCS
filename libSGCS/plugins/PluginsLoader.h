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
#ifndef PLUGINSLOADER_H
#define PLUGINSLOADER_H

#include "DataSourcePlugin.h"
#include "ProtocolPlugin.h"
#include <QDir>
#include <QObject>
#include <QPluginLoader>

namespace sgcs
{
namespace plugin
{
class PluginsLoader : public QObject
{
    Q_OBJECT
public:
    explicit PluginsLoader(QObject *parent = nullptr);

    bool load(const QDir &pluginsDir);

    QList<ProtocolPlugin *> protocols() const;

    QList<DataSourcePlugin *> datasources() const;

private:
    QList<ProtocolPlugin *> _protocol;
    QList<DataSourcePlugin *> _datasources;

signals:
};
}
}
#endif // PLUGINSLOADER_H
