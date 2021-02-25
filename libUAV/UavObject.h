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
#ifndef UAVOBJECT_H
#define UAVOBJECT_H
#include "UavMessage.h"
#include <boost/log/trivial.hpp>
#include <cmath>
#include <memory>
#include <numeric>

namespace uav
{
template <class T>
class UavObject
{
public:
    explicit UavObject() : m_has(0)
    {
    }

    bool has(T h) const
    {
        return m_has & h;
    }
    void setHas(T h)
    {
        m_has |= h;
    }
    void rmHas(T h)
    {
        m_has = m_has & ~h;
    }

protected:
    T m_has = 0;
};
}
#endif // UAVOBJECT_H
