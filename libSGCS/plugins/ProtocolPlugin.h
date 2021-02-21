#ifndef PROTOCOLPLUGIN_H
#define PROTOCOLPLUGIN_H

#include "PluginInterface.h"
#include <UavProtocol.h>

namespace sgcs
{
namespace plugin
{
class ProtocolPlugin : public PluginInterface
{
public:
    explicit ProtocolPlugin();
    virtual ~ProtocolPlugin() = default;

    virtual uav::UavProtocol *instance() = 0;
};
}
}
#endif // PROTOCOLPLUGIN_H
