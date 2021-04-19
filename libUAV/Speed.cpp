#include "Speed.h"

namespace uav
{
Speed::Speed() : m_ground(0.f), m_air(0.f)
{
}

Speed::~Speed()
{
}

void Speed::addCallback(Speed::OnChangeSpeedCallback *cb)
{
    m_callbacks.push_back(cb);
}

void Speed::removeCallback(Speed::OnChangeSpeedCallback *cb)
{
    m_callbacks.remove(cb);
}

void Speed::ground(float &v)
{
    std::lock_guard grd(m_speedLock);
    v = m_ground;
}

void Speed::setGround(float ground)
{
    std::lock_guard grd(m_speedLock);
    if (std::abs(m_ground - ground) < 0.01)
        return;
    m_ground = ground;
}

void Speed::air(float &a)
{
    std::lock_guard grd(m_speedLock);
    a = m_air;
}

void Speed::doSendSpeed(float newSpeed)
{
    for (auto o : m_callbacks)
        o->sendSpeed(newSpeed);
}

void Speed::setAir(float air)
{
    std::lock_guard grd(m_speedLock);
    if (std::abs(m_air - air) < 0.01)
        return;
    m_air = air;
}
}
