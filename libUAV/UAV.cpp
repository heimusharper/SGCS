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
UAV::UAV()
: UavObject()
, m_id(-1)
, m_type(UAVType::UNDEFINED)
, m_ahrs(new AHRS())
, m_gps(new GPS())
, m_position(new Position())
, m_home(new Home())
, m_power(new Power())
, m_speed(new Speed())
, m_mission(new Mission())
, m_status(new Status())
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
}

void UAV::process(std::unique_ptr<UavTask> message)
{
    UavTask *task = message.get();
    if (UAV::Message *uavmessage = dynamic_cast<UAV::Message *>(task))
    {
        if (uavmessage->id.dirty())
            setId(uavmessage->id.get());
        if (uavmessage->type.dirty())
            setType(uavmessage->type.get());
    }
    else if (UAV::MessageFlight *uavmessage = dynamic_cast<UAV::MessageFlight *>(task))
    {
        if (uavmessage->state.dirty())
            setState(uavmessage->state.get());
    }
    else if (Home::Message *homemsg = dynamic_cast<Home::Message *>(task))
        m_home->process(homemsg);
    else if (Power::Message *poewermsg = dynamic_cast<Power::Message *>(task))
        m_power->process(poewermsg);
    else if (Speed::Message *speedmsg = dynamic_cast<Speed::Message *>(task))
        m_speed->process(speedmsg);
    else if (Status::Message *statmsg = dynamic_cast<Status::Message *>(task))
        m_status->process(statmsg);
}
int UAV::id() const
{
    return m_id;
}

UAVType UAV::type() const
{
    return m_type;
}

void UAV::setId(int id)
{
    if (m_id == id)
        return;
    BOOST_LOG_TRIVIAL(info) << "UAV ID " << id;
    m_id = id;
}

void UAV::setType(UAVType type)
{
    if (m_type == type)
        return;
    BOOST_LOG_TRIVIAL(info) << "UAV TYPE " << (int)type;
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

UAVControlState UAV::state() const
{
    return m_state;
}

void UAV::sendControlState(UAVControlState newState, bool force)
{
    for (auto c : m_controls)
        c->changeState(newState, force);
}

void UAV::setState(const UAVControlState &state)
{
    if (m_state != state)
    {
        m_state = state;
        for (auto c : m_controls)
            c->onChangeControlState(m_state);
    }
}

Status *UAV::status() const
{
    return m_status;
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
