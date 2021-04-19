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
class UavSendMessage
{
public:
    enum class Priority
    {
        HIGHT,
        NORMAL,
        LOW
    };

    UavSendMessage(int ticks = 1, int interval = 0, UavSendMessage::Priority priority = UavSendMessage::Priority::NORMAL);
    virtual ~UavSendMessage()           = default;
    virtual tools::CharMap pack() const = 0;
    virtual bool empty() const          = 0;
    virtual void pop();

    void touch();
    bool isReadyToDelete();
    bool isReadyInterval() const;

    UavSendMessage::Priority priority() const;

    enum class CompareMode
    {
        HARD,
        LIGHT
    };

    virtual bool compare(UavSendMessage *message, CompareMode cmpmode)
    {
        switch (cmpmode)
        {
            case CompareMode::HARD:
                if (m_priority != message->priority())
                    return false;
                break;
            case CompareMode::LIGHT:
            default:
                break;
        }
        return compare(message);
    }

protected:
    virtual bool compare(const UavSendMessage *message) const = 0;

private:
    void reset();

    bool m_first = true;
    int m_interval;
    int m_ticks;
    const int m_ticksConst;
    Priority m_priority;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> m_sendTime;
};
}

#endif // UAVMESSAGE_H
