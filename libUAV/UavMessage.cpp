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
#include "UavMessage.h"

namespace uav
{
UavTask::UavTask(int target) : targetID(target)
{
}

UavSendMessage::UavSendMessage(int ticks, int interval, UavSendMessage::Priority priority)
: m_first(true), m_interval(interval), m_ticks(ticks), m_priority(priority), m_sendTime(std::chrono::_V2::system_clock::now())
{
}

void UavSendMessage::touch()
{
    m_first = false;
    if (m_ticks > 0)
        m_ticks--;
    m_sendTime = std::chrono::_V2::system_clock::now();
}

bool UavSendMessage::isReadyToDelete() const
{
    return (!m_first && (m_ticks == 0));
}

bool UavSendMessage::isReadyInterval() const
{
    if (m_first)
        return true;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::_V2::system_clock::now() - m_sendTime);
    return ms.count() >= m_interval;
}

UavSendMessage::Priority UavSendMessage::priority() const
{
    return m_priority;
}

}
