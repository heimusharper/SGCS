#include "Power.h"

namespace uav
{
Power::Power() : m_voltage(0)
{
}

Power::~Power()
{
}

void Power::voltage(float &v)
{
    std::lock_guard grd(m_voltageLock);
    v = m_voltage;
}

void Power::setVoltage(float voltage)
{
    std::lock_guard grd(m_voltageLock);
    if (std::abs(m_voltage - voltage) < 0.01)
        return;
    m_voltage = voltage;
}

}
