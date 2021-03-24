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

void Position::process(Position::Message *message)
{
    if (has(uav::Position::HAS::HAS_SOURCE_GPS))
    {
        if (message->lat.dirty() || message->lon.dirty() || message->alt.dirty())
        {
            geo::Coords3D c3d(message->lat.get(), message->lon.get(), message->alt.get());
            setGps(std::move(c3d));
        }
    }
}

geo::Coords3D Position::gps() const
{
    return m_gps.get();
}

void Position::setGps(geo::Coords3D &&gps)
{
    if (m_gps == gps)
        return;
    m_gps = gps;
    for (auto c : m_callbacks)
        c->updateGPS();
    BOOST_LOG_TRIVIAL(info) << "GPS POS {" << m_gps.get().lat() << "; " << m_gps.get().lon() << "; " << m_gps.get().alt() << "}";
}

void Position::setControl(PositionControlInterface *control)
{
    _control = control;
    if (_control)
        setHas(uav::Position::HAS::HAS_POSITION_CONTROL);
    else
        rmHas(uav::Position::HAS::HAS_POSITION_CONTROL);
}
bool Position::goTo(geo::Coords3D &&target)
{
    if (_control && has(uav::Position::HAS::HAS_POSITION_CONTROL))
        return _control->goTo(std::move(target));
    return false;
}

void Position::addCallback(Position::OnChangePositionCallback *c)
{
    m_callbacks.push_back(c);
}

void Position::removeCallback(Position::OnChangePositionCallback *c)
{
    m_callbacks.remove(c);
}

}
