#ifndef LEAFINTERFACE_H
#define LEAFINTERFACE_H
#include "PluginInterface.h"
#include <UAV.h>

namespace gcs
{
class LeafInterface : public sgcs::plugin::PluginInterface
{
public:
    LeafInterface();

    virtual void setUAV(uav::UAV *uav) final;

protected:
    virtual void onSetUAV() = 0;

public:
    uav::UAV *_uav = nullptr;
};
}

#endif // LEAFINTERFACE_H
