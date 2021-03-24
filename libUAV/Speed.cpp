#include "Speed.h"

namespace uav
{
Speed::Speed() : m_ground(0.f), m_air(0.f)
{
}

Speed::~Speed()
{
}

void Speed::process(Speed::Message *message)
{
    if (has(HAS_GROUND_SPEED))
        if (message->air.dirty())
            setAir(message->air.get());
    if (has(HAS_AIR_SPEED))
        if (message->ground.dirty())
            setGround(message->ground.get());
}

void Speed::addCallback(Speed::OnChangeSpeedCallback *cb)
{
    m_callbacks.push_back(cb);
}

void Speed::removeCallback(Speed::OnChangeSpeedCallback *cb)
{
    m_callbacks.remove(cb);
}

float Speed::ground() const
{
    return m_ground;
}

void Speed::setGround(float ground)
{
    if (std::abs(m_ground - ground) < 0.01)
        return;
    m_ground = ground;
    for (auto o : m_callbacks)
        o->updateSpeed();
}

float Speed::air() const
{
    return m_air;
}

void Speed::setAir(float air)
{
    if (std::abs(m_air - air) < 0.01)
        return;
    m_air = air;
    for (auto o : m_callbacks)
        o->updateSpeed();
}
}
