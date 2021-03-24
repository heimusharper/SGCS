#ifndef UAVPOWER_H
#define UAVPOWER_H
#include "UavObject.h"
#include <Optional.h>
#include <cmath>
#include <list>

namespace uav
{
class Power : public UavObject<uint8_t>
{
public:
    class Message : public UavTask
    {
    public:
        Message(int target) : UavTask(target), voltage(0.f)
        {
        }
        tools::optional<float> voltage;
    };
    class OnChangePowerCallback
    {
    public:
        virtual void updateVoltage() = 0;
    };

    Power();
    virtual ~Power();

    void process(Power::Message *message);

    float voltage() const;

    // callback
    void addCallback(OnChangePowerCallback *cb);
    void removeCallback(OnChangePowerCallback *cb);

private:
    void setVoltage(float voltage);

private:
    tools::optional<float> m_voltage = 0;

    // callback
    std::list<OnChangePowerCallback *> m_callbacks;
};
}

#endif // UAVPOWER_H
