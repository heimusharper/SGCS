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
#include <list>

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

        virtual bool goTo(geo::Coords3D &&target) = 0;
    };

    class Message : public UavTask
    {
    public:
        Message(int target) : UavTask(target), lat(0.), lon(0.), alt(0.)
        {
        }
        tools::optional<double> lat;
        tools::optional<double> lon;
        tools::optional<double> alt;
    };
    class OnChangePositionCallback
    {
    public:
        virtual void updateGPS()
        {
        }
    };

    Position();
    virtual ~Position();

    void process(Position::Message *message);

    geo::Coords3D gps() const;

    // control
    void setControl(PositionControlInterface *control);
    bool goTo(geo::Coords3D &&target);

    void addCallback(OnChangePositionCallback *c);
    void removeCallback(OnChangePositionCallback *c);

protected:
    void setGps(geo::Coords3D &&gps);

private:
    tools::optional<geo::Coords3D> m_gps;

    PositionControlInterface *_control = nullptr;

    std::list<OnChangePositionCallback *> m_callbacks;
};
}

#endif // POSITION_H
