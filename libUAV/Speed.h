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

    class Message : public UavTask
    {
    public:
        Message(int target) : UavTask(target), ground(0.f), air(0.f)
        {
        }
        tools::optional<float> ground;
        tools::optional<float> air;
    };
    class OnChangeSpeedCallback
    {
    public:
        virtual void updateSpeed()
        {
        }

        virtual void sendSpeed(float newSpeed)
        {
        }
    };

    Speed();
    virtual ~Speed();

    void process(Speed::Message *message);

    // callback
    void addCallback(OnChangeSpeedCallback *cb);
    void removeCallback(OnChangeSpeedCallback *cb);

    float ground() const;
    float air() const;

    // do
    void doSendSpeed(float newSpeed);

protected:
    void setGround(float ground);
    void setAir(float air);

private:
    float m_ground;
    float m_air;

    // callback
    std::list<OnChangeSpeedCallback *> m_callbacks;
};
}

#endif // SPEED_H
