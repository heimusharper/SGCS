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
    else if (AHRS::Message *ahrsmsg = dynamic_cast<AHRS::Message *>(task))
        m_ahrs->process(ahrsmsg);
    else if (GPS::Message *gpsmsg = dynamic_cast<GPS::Message *>(task))
        m_gps->process(gpsmsg);
    else if (Position::Message *posgpsmsg = dynamic_cast<Position::Message *>(task))
        m_position->process(posgpsmsg);
    else if (Home::Message *homemsg = dynamic_cast<Home::Message *>(task))
        m_home->process(homemsg);
    else if (Power::Message *poewermsg = dynamic_cast<Power::Message *>(task))
        m_power->process(poewermsg);
    else if (Speed::Message *speedmsg = dynamic_cast<Speed::Message *>(task))
        m_speed->process(speedmsg);
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
