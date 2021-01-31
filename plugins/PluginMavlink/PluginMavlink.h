#ifndef PLUGINMAVLINK_H
#define PLUGINMAVLINK_H

#include "MavlinkProtocol.h"
#include <QObject>
#include <plugins/ProtocolPlugin.h>

class PluginMavlink : public sgcs::plugin::ProtocolPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ru.heimusharper.SGCS.ProtocolPlugin" FILE "meta.json")
    Q_INTERFACES(sgcs::plugin::ProtocolPlugin)
public:
    explicit PluginMavlink(QObject *parent = nullptr);
    virtual ~PluginMavlink();

    QString name() const override;
    virtual uav::UavProtocol *instance() override;

signals:
};

#endif // PLUGINMAVLINK_H
