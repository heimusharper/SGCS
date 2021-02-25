#include "AHRS.h"

namespace uav
{
AHRS::AHRS() : UavObject()
{
}

AHRS::~AHRS()
{
}

void AHRS::process(AHRS::Message *message)
{
    if (message->roll.dirty())
        setRoll(message->roll.get());
    if (message->pitch.dirty())
        setPitch(message->pitch.get());
    if (message->yaw.dirty())
        setYaw(message->yaw.get());
}

void AHRS::get(float &roll, float &pitch, float &yaw)
{
    roll  = m_roll;
    pitch = m_pitch;
    yaw   = m_yaw;
}

void AHRS::setRoll(float roll)
{
    if (fabs(m_roll - roll) < 0.1)
        return;
    m_roll = roll;
    BOOST_LOG_TRIVIAL(info) << "ROLL " << m_roll;
}

void AHRS::setPitch(float pitch)
{
    if (fabs(m_pitch - pitch) < 0.1)
        return;
    m_pitch = pitch;
    BOOST_LOG_TRIVIAL(info) << "PITCH " << m_pitch;
}

void AHRS::setYaw(float yaw)
{
    if (fabs(m_yaw - yaw) < 0.1)
        return;
    m_yaw = yaw;
    BOOST_LOG_TRIVIAL(info) << "YAW " << m_yaw;
}

}
