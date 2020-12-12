#include "PluginSerialConnection.h"

PluginSerialConnection::PluginSerialConnection()
{
}

PluginSerialConnection::~PluginSerialConnection()
{
}

bool PluginSerialConnection::load() const
{
    return RunConfiguration::instance().get<PluginSerialConnectionSymbol>()->load();
}

QString PluginSerialConnection::name() const
{
    return "PluginSerialConnection";
}

PluginSerialConnectionSymbol::PluginSerialConnectionSymbol(ConfigInterface *parent) : ConfigInterface(parent)
{
}

QString PluginSerialConnectionSymbol::name() const
{
    return "PluginSerialConnectionSymbol";
}

YAML::Node PluginSerialConnectionSymbol::toNode(const YAML::Node &file) const
{
    YAML::Node node = file;
    node["load"]    = m_load;
    return node;
}

void PluginSerialConnectionSymbol::fromNode(const YAML::Node &node)
{
    if (node["load"])
        m_load = node["load"].as<bool>();
    else
        m_load = true;
}

bool PluginSerialConnectionSymbol::load() const
{
    return m_load;
}

void PluginSerialConnectionSymbol::setLoad(bool load)
{
    m_load = load;
}
