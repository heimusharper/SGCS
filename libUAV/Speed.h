#ifndef UAVSPEED_H
#define UAVSPEED_H
#include "UavObject.h"
#include <Optional.h>
#include <cmath>
#include <list>

namespace uav
{
class Speed : public UavObject<uint8_t>
{
public:
    enum HAS : uint8_t
    {
        HAS_GROUND_SPEED = 0b1 << 0,
        HAS_AIR_SPEED    = 0b1 << 1,
    };

    class OnChangeSpeedCallback
    {
    public:
        virtual void sendSpeed(float newSpeed)
        {
        }
    };

    Speed();
    virtual ~Speed();

    // callback
    void addCallback(OnChangeSpeedCallback *cb);
    void removeCallback(OnChangeSpeedCallback *cb);

    void ground(float &v);
    void air(float &a);
    void setGround(float ground);
    void setAir(float air);

    // do
    void doSendSpeed(float newSpeed);

private:
    std::mutex m_speedLock;
    float m_ground;
    float m_air;

    // callback
    std::list<OnChangeSpeedCallback *> m_callbacks;
};
}

#endif // SPEED_H
