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

namespace uav
{
class UavTask
{
public:
    template <class T>
    class optional
    {
    public:
        optional(T def) : m_dirty(false), m_value(def)
        {
        }

        T &get()
        {
            m_dirty = true;
            return m_value;
        }
        bool dirty()
        {
            return m_dirty;
        }

        optional<T> &operator=(const T &other) noexcept
        {
            get() = other;
            return *this;
        }

    private:
        bool m_dirty = false;
        T m_value;
    };

    UavTask();
    virtual ~UavTask() = default;
};
class UavSendMessage
{
public:
    UavSendMessage();
};
}

#endif // UAVMESSAGE_H
