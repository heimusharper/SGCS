#include "IPConfig.h"

IPConfig::IPConfig(ConfigInterface *parent) : ConfigInterface(parent), m_port(0)
{
}

std::string IPConfig::name() const
{
    return "SerialConnectionConfiguration";
}

YAML::Node IPConfig::toNode(const YAML::Node &file) const
{
    YAML::Node node   = file;
    node["host_name"] = m_hostName;
    node["port"]      = m_port;
    return node;
}

void IPConfig::fromNode(const YAML::Node &node)
{
    if (node["host_name"])
        m_hostName = node["host_name"].as<std::string>();
    else
        m_hostName = "";
    if (node["port"])
        m_port = node["port"].as<uint16_t>();
    else
        m_port = 0;
}

std::string IPConfig::hostName() const
{
    return m_hostName;
}

void IPConfig::setHostName(const std::string &hostName)
{
    m_hostName = hostName;
}

uint16_t IPConfig::port() const
{
    return m_port;
}

void IPConfig::setPort(const uint16_t &porti)
{
    m_port = porti;
}
