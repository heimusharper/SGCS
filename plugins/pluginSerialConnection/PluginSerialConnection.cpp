#include "PluginSerialConnection.h"

PluginSerialConnection::PluginSerialConnection()
{
}

PluginSerialConnection::~PluginSerialConnection()
{
}

bool PluginSerialConnection::isLoad() const
{
    return RunConfiguration::instance().get<PluginSerialConnectionSymbol>()->load();
}

QString PluginSerialConnection::name() const
{
    return "PluginSerialConnection";
}

bool PluginSerialConnection::create()
{
    if (_window)
    {
        _window->addEntry(R"(import QtQuick.Controls 2.12;
ItemDelegate {
    text: qsTr("NewPlugin")
    objectName: "NewPluginButton"
    width: parent.width
    onClicked: {
        stack.push(serialConnection)
        drawer.close()
    }
})",
                          MainWindow::TargetComponent::DRAWER_LIST);
        QFile file(":/SerialConnectionView.qml");
        if (file.open(QIODevice::ReadOnly))
        {
            _window->addEntry(file.readAll(), MainWindow::TargetComponent::ROOT);
            file.close();
        }
        else
        {
            qWarning() << file.errorString();
        }
    }
    return true;
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
