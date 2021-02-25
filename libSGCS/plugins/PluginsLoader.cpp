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

namespace sgcs
{
namespace plugin
{
PluginsLoader::PluginsLoader()
{
}

bool PluginsLoader::load(const std::filesystem::path &pluginsDir)
{
    for (auto &fileName : std::filesystem::directory_iterator(pluginsDir))
    {
        void *handle = dlopen(fileName.path().c_str(), RTLD_LAZY);
        if (handle == NULL)
        {
            BOOST_LOG_TRIVIAL(warning) << "Undefined dlload" << fileName.path();
            continue;
        }
        dlerror();
        PluginInterface *(*plugin_func)();
        plugin_func = reinterpret_cast<PluginInterface *(*)()>(dlsym(handle, "dlload"));
        if (plugin_func == NULL)
        {
            BOOST_LOG_TRIVIAL(warning) << "failed handle dlload fuction from" << fileName.path();
            dlclose(handle);
            continue;
        }
        PluginInterface *plugin = plugin_func();

        ProtocolPlugin *protocolPlugin     = dynamic_cast<ProtocolPlugin *>(plugin);
        DataSourcePlugin *datasourcePlugin = dynamic_cast<DataSourcePlugin *>(plugin);
        LeafPlugin *leafPlugin             = dynamic_cast<LeafPlugin *>(plugin);

        BOOST_LOG_TRIVIAL(info) << "loaded " << fileName.path() << " PROTO " << protocolPlugin << " DATASOURCE "
                                << datasourcePlugin << " LEAF " << leafPlugin;
        if (protocolPlugin)
            _protocol.push_back(protocolPlugin);
        else if (datasourcePlugin)
            _datasources.push_back(datasourcePlugin);
        else if (leafPlugin)
            _leafs.push_back(leafPlugin);
    }
    return true;
}

std::vector<ProtocolPlugin *> PluginsLoader::protocols() const
{
    return _protocol;
}

std::vector<DataSourcePlugin *> PluginsLoader::datasources() const
{
    return _datasources;
}

std::vector<LeafPlugin *> PluginsLoader::leafs() const
{
    return _leafs;
}
}
}
