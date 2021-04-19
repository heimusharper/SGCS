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
#include "Position.h"

namespace uav
{
Position::Position() : UavObject(), m_gps(geo::Coords3D())
{
}

Position::~Position()
{
}

void Position::gps(geo::Coords3D &coord)
{
    std::lock_guard grd(m_gpsLock);
    coord = m_gps;
}

void Position::setGps(const geo::Coords3D &gps)
{
    std::lock_guard grd(m_gpsLock);
    if (m_gps == gps)
        return;
    m_gps = gps;
    // for (auto c : m_control)
    //    c->updateGPS();
    // BOOST_LOG_TRIVIAL(info) << "GPS POS {" << m_gps.get().lat() << "; " << m_gps.get().lon() << "; " << m_gps.get().alt() << "}";
}

void Position::setControl(PositionControlInterface *control)
{
    m_control.push_back(control);
    if (!m_control.empty())
        setHas(uav::Position::HAS::HAS_POSITION_CONTROL);
    else
        rmHas(uav::Position::HAS::HAS_POSITION_CONTROL);
}
bool Position::goTo(geo::Coords3D &&target, const geo::Coords3D &base)
{
    if (has(uav::Position::HAS::HAS_POSITION_CONTROL))
    {
        for (auto c : m_control)
        {
            c->goTo(std::move(target), base);
        }
        return true;
    }
    return false;
}
}
