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
#include "PluginsLoader.h"

PluginsLoader::PluginsLoader(QObject *parent) : QObject(parent)
{
}

bool PluginsLoader::load(const QDir &pluginsDir)
{
    const QStringList entries = pluginsDir.entryList(QDir::Files);
    for (const QString &fileName : entries)
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
            SgcsPlugin *sgcs = qobject_cast<SgcsPlugin *>(plugin);
            if (sgcs)
            {
                const QMetaObject *meta = sgcs->metaObject();
                qDebug() << "ON LOADED" << QString(meta->className());
                return true;
            }
            pluginLoader.unload();
        }
        else
        {
            qWarning() << "Failed load plugin " << pluginLoader.errorString();
        }
    }

    return false;
}
