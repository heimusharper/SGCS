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
#include <UAV.h>

namespace uav
{
UAV::UAV(int id)
: UavObject()
, m_id(id)
, m_type(UAVType::UNDEFINED)
, m_ahrs(new AHRS())
, m_gps(new GPS())
, m_position(new Position())
, m_home(new Home())
, m_power(new Power())
, m_speed(new Speed())
, m_mission(new Mission())
, m_status(new Status())
, m_calibration(new Calibration())
, m_state(UAVControlState::WAIT)
, m_takeoffAltitude(10)
{
}

UAV::~UAV()
{
    delete m_ahrs;
    delete m_gps;
    delete m_position;
    delete m_home;
    delete m_power;
    delete m_speed;
    delete m_mission;
    delete m_status;
    delete m_calibration;
}

int UAV::id() const
{
    return m_id;
}

void UAV::type(UAVType &type)
{
    std::lock_guard grd(m_typeLock);
    type = m_type;
}

void UAV::setType(UAVType type)
{
    std::lock_guard grd(m_typeLock);
    if (m_type == type)
        return;
    m_type = type;
}

void UAV::addControl(UAV::ControlInterface *i)
{
    m_controls.push_back(i);
}

void UAV::removeControl(UAV::ControlInterface *i)
{
    m_controls.remove(i);
}

void UAV::state(UAVControlState &s)
{
    std::lock_guard grd(m_typeLock);
    s = m_state;
}

void UAV::sendControlState(UAVControlState newState, bool force)
{
    for (auto c : m_controls)
        c->changeState(newState, force);
}

void UAV::setState(const UAVControlState &state)
{
    std::lock_guard grd(m_typeLock);
    if (m_state == state)
        return;
    m_state = state;
    for (auto c : m_controls)
        c->onChangeControlState(m_state);
}

Status *UAV::status() const
{
    return m_status;
}

Calibration *UAV::calibration() const
{
    return m_calibration;
}

Mission *UAV::mission() const
{
    return m_mission;
}

int UAV::takeoffAltitude() const
{
    return m_takeoffAltitude;
}

void UAV::setTakeoffAltitude(int takeoffAltitude)
{
    m_takeoffAltitude = takeoffAltitude;
}

Speed *UAV::speed() const
{
    return m_speed;
}

Power *UAV::power() const
{
    return m_power;
}

Home *UAV::home() const
{
    return m_home;
}

Position *UAV::position() const
{
    return m_position;
}

GPS *UAV::gps() const
{
    return m_gps;
}

AHRS *UAV::ahrs() const
{
    return m_ahrs;
}
}
