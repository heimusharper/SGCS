#include "LeafInterface.h"

namespace gcs
{
LeafInterface::LeafInterface() : sgcs::plugin::PluginInterface()
{
}

void LeafInterface::setUAV(uav::UAV *uav)
{
    _uav = uav;
    onSetUAV();
}
}
