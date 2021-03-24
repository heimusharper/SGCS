/*
 * This file is part of the SGCS distribution (https://github.com/heimusharper/SGCS).
 * Copyright (c) 2020 Andrey Stepanov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "AHRS.h"

namespace uav
{
AHRS::AHRS() : UavObject(), m_roll(0), m_pitch(0), m_yaw(0)
{
}

AHRS::~AHRS()
{
}

void AHRS::process(AHRS::Message *message)
{
    bool update = false;
    if (message->roll.dirty())
        if (setRoll(message->roll.get()))
            update = true;
    if (message->pitch.dirty())
        if (setPitch(message->pitch.get()))
            update = true;
    if (message->yaw.dirty())
        if (setYaw(message->yaw.get()))
            update = true;
    if (update)
        for (auto c : m_callbacks)
            c->updateAngles();
}

void AHRS::get(float &roll, float &pitch, float &yaw)
{
    roll  = m_roll;
    pitch = m_pitch;
    yaw   = m_yaw;
}

bool AHRS::setRoll(float roll)
{
    if (fabs(m_roll - roll) < 0.1)
        return false;
    m_roll = roll;
    return true;
    // BOOST_LOG_TRIVIAL(info) << "ROLL " << m_roll;
}

bool AHRS::setPitch(float pitch)
{
    if (fabs(m_pitch - pitch) < 0.1)
        return false;
    m_pitch = pitch;
    return true;
    // BOOST_LOG_TRIVIAL(info) << "PITCH " << m_pitch;
}

bool AHRS::setYaw(float yaw)
{
    if (fabs(m_yaw - yaw) < 0.1)
        return false;
    m_yaw = yaw;
    return true;
    // BOOST_LOG_TRIVIAL(info) << "YAW " << m_yaw;
}

}
