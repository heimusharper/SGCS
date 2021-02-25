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
