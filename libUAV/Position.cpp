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
Position::Position() : UavObject()
{
}

Position::~Position()
{
}

void Position::process(Position::MessageGPS *message)
{
    if (has(uav::Position::HAS::HAS_SOURCE_GPS))
    {
        if (message->lat.dirty() || message->lon.dirty() || message->alt.dirty())
        {
            geo::Coords3D c3d(message->lat.get(), message->lon.get(), message->alt.get());
            setGps(c3d);
        }
    }
}

geo::Coords3D<double> Position::gps() const
{
    return _gps;
}

void Position::setGps(const geo::Coords3D<double> &gps)
{
    if (_gps == gps)
        return;
    _gps = gps;
    BOOST_LOG_TRIVIAL(info) << "GPS POS {" << _gps.lat() << "; " << _gps.lon() << "; " << _gps.alt() << "}";
}

}
