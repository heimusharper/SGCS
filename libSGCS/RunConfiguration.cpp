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
#include "RunConfiguration.h"

ConfigInterface::ConfigInterface(ConfigInterface *root) : _parent(root)
{
}

ConfigInterface *ConfigInterface::parent() const
{
    return _parent;
}

void ConfigInterface::clear()
{
    while (!_nodes.isEmpty())
    {
        auto obj = _nodes.takeFirst();
        obj->clear();
        delete obj;
    }
}

void ConfigInterface::save()
{
    if (_parent)
        _parent->save();
}

YAML::Node ConfigInterface::nodeAt(ConfigInterface *node)
{
    ConfigInterface *lastRoot = node;
    ConfigInterface *nowRoot  = node->parent();
    QStringList tree;
    tree.append(node->name());
    do
    {
        if (nowRoot)
        {
            tree.append(nowRoot->name());
            lastRoot = nowRoot;
            nowRoot  = nowRoot->parent();
        }
        else
        {
            if (lastRoot)
            {
                auto rc = dynamic_cast<RunConfiguration *>(lastRoot);
                if (rc)
                    return rc->getFromFile(tree);
            }
            break;
        }
    } while (true);
    YAML::Node x;
    return x;
}

//

ApplicationConfiguration::ApplicationConfiguration(ConfigInterface *parent) : ConfigInterface(parent)
{
}

QString ApplicationConfiguration::name() const
{
    return "ApplicationConfiguration";
}

YAML::Node ApplicationConfiguration::toNode(const YAML::Node &file) const
{
    YAML::Node node          = file;
    node["run_profile_name"] = m_profile.toStdString();
    return node;
}

void ApplicationConfiguration::fromNode(const YAML::Node &node)
{
    if (node["run_profile_name"])
        m_profile = QString::fromStdString(node["run_profile_name"].as<std::string>());
    else
        m_profile = "default";
}

QString ApplicationConfiguration::profile() const
{
    return m_profile;
}

void ApplicationConfiguration::setProfile(const QString &profile)
{
    m_profile = profile;
}

//
RunConfiguration::RunConfiguration(ConfigInterface *root) : ConfigInterface(root)
{
}

RunConfiguration::~RunConfiguration()
{
    save();
    clear();
}

RunConfiguration &RunConfiguration::instance()
{
    static RunConfiguration rc;
    return rc;
}

bool RunConfiguration::create(const QString &filename)
{
    _filename = filename;
    try
    {
        _yaml = YAML::LoadFile(filename.toStdString());
        fromNode(_yaml);
        return true;
    }
    catch (YAML::Exception &e)
    {
        qWarning() << "Exception " << filename << QString(e.what());
    }
    return false;
}

QString RunConfiguration::name() const
{
    return "RunConfiguration";
}

YAML::Node RunConfiguration::toNode(const YAML::Node &file) const
{
    YAML::Node root = file;
    for (const ConfigInterface *node : _nodes)
        root[node->name().toStdString()] = node->toNode(root[node->name().toStdString()]);
    return root;
}

void RunConfiguration::fromNode(const YAML::Node &node)
{
    for (ConfigInterface *i : _nodes)
        if (node[i->name().toStdString()])
            i->fromNode(node[i->name().toStdString()]);
}

YAML::Node RunConfiguration::getFromFile(const QStringList &tree)
{
    try
    {
        YAML::Node config                                    = _yaml;
        QList<QString>::const_reverse_iterator constIterator = tree.rbegin();
        for (; constIterator != tree.rend(); ++constIterator)
            if (config[(*constIterator).toStdString()])
                config = config[(*constIterator).toStdString()];
        return config;
    }
    catch (YAML::BadFile &e)
    {
        save();
        return getFromFile(tree);
    }
    YAML::Node r;
    return r;
}

void RunConfiguration::forceSave()
{
    save();
}

void RunConfiguration::save()
{
    YAML::Node rootNode                  = _yaml;
    rootNode[this->name().toStdString()] = toNode(rootNode[this->name().toStdString()]);
    std::ofstream fout(_filename.toStdString());
    fout << rootNode;
}
