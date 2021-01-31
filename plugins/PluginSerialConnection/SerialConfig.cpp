#include "SerialConfig.h"

SerialConfig::SerialConfig(ConfigInterface *parent) : ConfigInterface(parent), m_portName(QString()), m_baudRate(0)
{
}

QString SerialConfig::name() const
{
    return "SerialConnectionConfiguration";
}

YAML::Node SerialConfig::toNode(const YAML::Node &file) const
{
    YAML::Node node   = file;
    node["port_name"] = m_portName.toStdString();
    node["port_baud"] = m_baudRate;
    return node;
}

void SerialConfig::fromNode(const YAML::Node &node)
{
    if (node["port_name"])
        m_portName = QString::fromStdString(node["port_name"].as<std::string>());
    else
        m_portName = "";
    if (node["port_baud"])
        m_baudRate = node["port_baud"].as<int>();
    else
        m_baudRate = 0;
}

QString SerialConfig::portName() const
{
    return m_portName;
}

void SerialConfig::setPortName(const QString &portName)
{
    m_portName = portName;
}

quint16 SerialConfig::baudRate() const
{
    return m_baudRate;
}

void SerialConfig::setBaudRate(const quint16 &baudRate)
{
    m_baudRate = baudRate;
}
