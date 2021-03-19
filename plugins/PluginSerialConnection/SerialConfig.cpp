#include "SerialConfig.h"

SerialConfig::SerialConfig(ConfigInterface *parent) : ConfigInterface(parent), m_baudRate(0)
{
}

std::string SerialConfig::name() const
{
    return "SerialConnectionConfiguration";
}

YAML::Node SerialConfig::toNode(const YAML::Node &file) const
{
    YAML::Node node   = file;
    node["port_name"] = m_portName;
    node["port_baud"] = m_baudRate;
    return node;
}

void SerialConfig::fromNode(const YAML::Node &node)
{
    if (node["port_name"])
        m_portName = node["port_name"].as<std::string>();
    else
        m_portName = "";
    if (node["port_baud"])
        m_baudRate = node["port_baud"].as<uint32_t>();
    else
        m_baudRate = 0;
}

std::string SerialConfig::portName() const
{
    return m_portName;
}

void SerialConfig::setPortName(const std::string &portName)
{
    m_portName = portName;
}

uint32_t SerialConfig::baudRate() const
{
    return m_baudRate;
}

void SerialConfig::setBaudRate(const uint32_t &baudRate)
{
    m_baudRate = baudRate;
}
