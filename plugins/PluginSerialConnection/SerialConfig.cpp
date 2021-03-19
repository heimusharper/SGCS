#include "SerialConfig.h"

SerialConfig::SerialConfig(ConfigInterface *parent)
: ConfigInterface(parent), m_portName(""), m_baudRate(0), m_autoName(true)
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
    node["auto"]      = m_autoName;
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
    if (node["auto"])
        m_autoName = node["auto"].as<bool>();
    else
        m_autoName = true;
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

bool SerialConfig::autoName() const
{
    return m_autoName;
}

void SerialConfig::setAutoName(bool autoName)
{
    m_autoName = autoName;
}
