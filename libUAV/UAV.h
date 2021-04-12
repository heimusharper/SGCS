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
#ifndef UAVUAV_H
#define UAVUAV_H
#include "AHRS.h"
#include "GPS.h"
#include "Home.h"
#include "Mission.h"
#include "Position.h"
#include "Power.h"
#include "Speed.h"
#include "Status.h"
#include "UavObject.h"

namespace uav
{
enum class UAVType
{
    PLANE,
    MULTICOPTER,
    VTOL,
    UNDEFINED
};

enum class UAVControlState
{
    WAIT,
    PREPARED,
    STARTED,
    AUTO,
    MANUAL_ONBOARD,
    MANUAL_OFFBOARD,
    RTL,
    LAND,
    KILL
};

class UAV : public UavObject<uint8_t>
{
public:
    class Message : public UavTask
    {
    public:
        Message(int target) : UavTask(target), id(-1), type(UAVType::UNDEFINED)
        {
        }
        tools::optional<int> id;
        tools::optional<UAVType> type;
    };

    class MessageFlight : public UavTask
    {
    public:
        MessageFlight(int target) : UavTask(target), state(UAVControlState::WAIT)
        {
        }
        tools::optional<UAVControlState> state;
    };

    class ControlInterface
    {
    public:
        virtual void changeState(UAVControlState state, bool force = false)
        {
        }
        virtual void onChangeControlState(UAVControlState state)
        {
        }
    };

    UAV();
    virtual ~UAV();
    void process(std::unique_ptr<uav::UavTask> message);

    int id() const;

    UAVType type() const;

    AHRS *ahrs() const;

    GPS *gps() const;

    Position *position() const;

    Home *home() const;

    Power *power() const;

    Speed *speed() const;

    Mission *mission() const;

    Status *status() const;

    int takeoffAltitude() const;
    void setTakeoffAltitude(int takeoffAltitude);

    //
    void addControl(ControlInterface *i);
    void removeControl(ControlInterface *i);

    UAVControlState state() const;

    // control

    void sendControlState(UAVControlState newState, bool force = false);

private:
    void setId(int id);

    void setType(UAVType type);

    void setState(const UAVControlState &state);

private:
    int m_id;

    UAVType m_type;

    // Objs

    AHRS *m_ahrs = nullptr;

    GPS *m_gps = nullptr;

    Position *m_position = nullptr;

    Home *m_home = nullptr;

    Power *m_power = nullptr;

    Speed *m_speed = nullptr;

    Mission *m_mission = nullptr;

    Status *m_status = nullptr;

    UAVControlState m_state;

    int m_takeoffAltitude;

    std::list<ControlInterface *> m_controls;
};
}
#endif
