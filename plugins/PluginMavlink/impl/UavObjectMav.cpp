#include "UavObjectMav.h"

UavObjectMav::UavObjectMav()
{
}

uav::UavAttitude *UavObjectMav::createAttitude() const
{
    return new UavAttitudeMav();
}
