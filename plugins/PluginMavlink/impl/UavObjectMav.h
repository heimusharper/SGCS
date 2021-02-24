#ifndef UAVOBJECTMAV_H
#define UAVOBJECTMAV_H
#include "UavAttitudeMav.h"
#include <obj/UavObject.h>

class UavObjectMav : public uav::UavObject
{
public:
    UavObjectMav();
    virtual uav::UavAttitude *createAttitude() const override final;
};

#endif // UAVOBJECTMAV_H
