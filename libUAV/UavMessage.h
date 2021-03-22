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
#ifndef UAVMESSAGE_H
#define UAVMESSAGE_H
#include <CharMap.h>
#include <Optional.h>
#include <vector>

namespace uav
{
class UavTask
{
public:
    UavTask(int target);
    virtual ~UavTask() = default;

    int targetID = 0;
};
class UavSendMessage
{
public:
    UavSendMessage(int ticks = 1, int interval = 0);
    virtual tools::CharMap pack() const = 0;

    void touch();
    bool isReadyToDelete() const;
    bool isReadyInterval() const;

private:
    bool m_first = true;
    int m_interval;
    int m_ticks;
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> m_sendTime;
};
}

#endif // UAVMESSAGE_H
