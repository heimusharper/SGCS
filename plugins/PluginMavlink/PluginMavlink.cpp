#include "PluginMavlink.h"

PluginMavlink::PluginMavlink(QObject *parent) : sgcs::plugin::ProtocolPlugin(parent)
{
}

PluginMavlink::~PluginMavlink()
{
}

QString PluginMavlink::name() const
{
    return tr("Mavlink");
}

uav::UavProtocol *PluginMavlink::instance()
{
    return new MavlinkProtocol();
}
