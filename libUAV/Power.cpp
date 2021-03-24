#include "Power.h"

namespace uav
{
Power::Power() : m_voltage(0)
{
}

Power::~Power()
{
}

void Power::process(uav::Power::Message *message)
{
    if (message->voltage.dirty())
        setVoltage(message->voltage.get());
}

float Power::voltage() const
{
    return m_voltage.get();
}

void Power::addCallback(Power::OnChangePowerCallback *cb)
{
    m_callbacks.push_back(cb);
}

void Power::removeCallback(Power::OnChangePowerCallback *cb)
{
    m_callbacks.remove(cb);
}

void Power::setVoltage(float voltage)
{
    if (m_voltage.dirty())
        if (std::abs(m_voltage.get() - voltage) < 0.01)
            return;
    m_voltage = voltage;
    for (auto c : m_callbacks)
        c->updateVoltage();
}

}
