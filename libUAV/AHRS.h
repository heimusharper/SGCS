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
#ifndef AHRS_H
#define AHRS_H
#include "UavObject.h"

namespace uav
{
class AHRS : public UavObject<uint16_t>
{
public:
    class Message : public UavMessage
    {
    public:
        Message() : UavMessage(), roll(0.f), pitch(0.f), yaw(0.f)
        {
        }
        Message::optional<float> roll;
        Message::optional<float> pitch;
        Message::optional<float> yaw;
    };

    AHRS();
    virtual ~AHRS();

    void process(AHRS::Message *message);

    void get(float &roll, float &pitch, float &yaw);

private:
    void setRoll(float roll);
    void setPitch(float pitch);
    void setYaw(float yaw);

private:
    float m_roll  = 0;
    float m_pitch = 0;
    float m_yaw   = 0;
};
}
#endif // AHRS_H
