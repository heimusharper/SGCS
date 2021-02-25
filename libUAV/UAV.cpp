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
UAV::UAV() : UavObject(), m_id(-1)
{
}

UAV::~UAV()
{
}

void UAV::process(uav::UavMessage *message)
{
    UAV::Message *thisMessage = dynamic_cast<UAV::Message *>(message); // TODO: fastef cast
    if (thisMessage)
    {
        if (thisMessage->id.dirty())
            setId(thisMessage->id.get());
    }
}
int UAV::id() const
{
    return m_id;
}

void UAV::setId(int id)
{
    if (m_id == id)
        return;

    BOOST_LOG_TRIVIAL(info) << "UAV ID" << id;

    m_id = id;
}
}
