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
    Power();
    virtual ~Power();

    void voltage(float &v);
    void setVoltage(float voltage);

private:
    std::mutex m_voltageLock;
    float m_voltage = 0;
};
}

#endif // UAVPOWER_H
