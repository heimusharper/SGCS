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
#ifndef UAVPOSITION_H
#define UAVPOSITION_H
#include "UavObject.h"
#include <geo/Coords3D.h>

namespace uav
{
class Position : public UavObject<uint16_t>
{
public:
    enum HAS : uint16_t
    {
        HAS_SOURCE_GPS       = 0b1 << 0,
        HAS_POSITION_CONTROL = 0b1 << 1
    };

    class PositionControlInterface
    {
    public:
        PositionControlInterface()
        {
        }

        virtual bool goTo(const geo::Coords3D<double> &target) = 0;
    };

    class MessageGPS : public UavTask
    {
    public:
        MessageGPS() : UavTask(), lat(0.), lon(0.), alt(0.)
        {
        }
        tools::optional<double> lat;
        tools::optional<double> lon;
        tools::optional<double> alt;
    };

    Position();
    virtual ~Position();

    void process(Position::MessageGPS *message);

    geo::Coords3D<double> gps() const;

    // control
    void setControl(PositionControlInterface *control);
    void goTo(const geo::Coords3D<double> &target);

protected:
    void setGps(const geo::Coords3D<double> &gps);

private:
    geo::Coords3D<double> _gps;

    PositionControlInterface *_control = nullptr;
};
}

#endif // POSITION_H
