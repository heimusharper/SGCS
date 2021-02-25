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
#ifndef UAV_H
#define UAV_H
#include "UavObject.h"

namespace uav
{
class UAV : public UavObject
{
public:
    class Message : public UavMessage
    {
    public:
        Message() : UavMessage(), id(-1)
        {
        }
        Message::optional<int> id;
    };

    UAV();
    virtual ~UAV();
    void process(uav::UavMessage *message);

    int id() const;

private:
    void setId(int id);

private:
    int m_id = -1;
};
}
#endif // SGCS_H
