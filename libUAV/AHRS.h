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
#ifndef UAVAHRS_H
#define UAVAHRS_H
#include "UavObject.h"
#include <list>

namespace uav
{
class AHRS : public UavObject<uint16_t>
{
public:
    class Message : public UavTask
    {
    public:
        Message(int target) : UavTask(target), roll(0.f), pitch(0.f), yaw(0.f)
        {
        }
        tools::optional<float> roll;
        tools::optional<float> pitch;
        tools::optional<float> yaw;
    };

    class OnChangeAHRSCallback
    {
    public:
        virtual void updateAngles()
        {
        }

        virtual void sendYaw(float yaw)
        {
        }
    };

    AHRS();
    virtual ~AHRS();

    void process(AHRS::Message *message);

    void get(float &roll, float &pitch, float &yaw);

    void addCallback(OnChangeAHRSCallback *call);
    void removeCallback(OnChangeAHRSCallback *call);

    void doSendYaw(float yaw);

private:
    bool setRoll(float roll);
    bool setPitch(float pitch);
    bool setYaw(float yaw);

private:
    float m_roll;
    float m_pitch;
    float m_yaw;

    std::list<OnChangeAHRSCallback *> m_callbacks;
};
}
#endif // UAVAHRS_H
