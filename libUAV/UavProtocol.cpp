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
#include "UavProtocol.h"

namespace uav
{
UavProtocol::UavProtocol()
{
}

boost::container::vector<uint8_t> UavProtocol::hello() const
{
    return boost::container::vector<uint8_t>();
}

void UavProtocol::setIsHasData(bool l)
{
    _hasData.store(l);
}

bool UavProtocol::isHasData() const
{
    return _hasData.load();
}
uav::UavMessage *UavProtocol::message()
{
    _messageStoreMutex.lock();
    uav::UavMessage *obj = _messages.front();
    _messages.pop_front();
    if (_messages.empty())
        setIsHasData(false);
    _messageStoreMutex.unlock();
    return obj;
}

}
