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

void AHRS::get(float &roll, float &pitch, float &yaw)
{
    std::lock_guard grd(m_anglesMutex);
    roll  = m_roll;
    pitch = m_pitch;
    yaw   = m_yaw;
}

void AHRS::set(const float &roll, const float &pitch, const float &yaw)
{
    {
        std::lock_guard grd(m_anglesMutex);
        if (std::abs(m_roll - roll) > 0.01 || std::abs(m_pitch - pitch) > 0.01 || std::abs(m_yaw - yaw) > 0.01)
        {
            m_roll  = roll;
            m_pitch = pitch;
            m_yaw   = yaw;
        }
        else
            return;
    }
    // for (auto c : m_callbacks)
    //    c->updateAngles();
}

void AHRS::addCallback(AHRS::OnChangeAHRSCallback *call)
{
    m_callbacks.push_back(call);
}

void AHRS::removeCallback(AHRS::OnChangeAHRSCallback *call)
{
    m_callbacks.remove(call);
}

void AHRS::doSendYaw(float yaw)
{
    for (auto x : m_callbacks)
        x->sendYaw(yaw);
}

}
