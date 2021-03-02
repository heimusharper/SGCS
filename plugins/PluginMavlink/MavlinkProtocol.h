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

#ifndef MAVLINKPROTOCOL_H
#define MAVLINKPROTOCOL_H

#include "ardupilotmega/mavlink.h"
#include "mavlink_types.h"
#include <UavProtocol.h>
#include <atomic>
#include <boost/log/trivial.hpp>
#include <queue>
#include <thread>

class MavlinkMessageType : public uav::UavSendMessage
{
public:
    MavlinkMessageType(const mavlink_message_t &mavlink) : m_mavlink(mavlink)
    {
    }

    mavlink_message_t mavlink() const;

private:
    mavlink_message_t m_mavlink;
};

class MavlinkPositionControl : public uav::Position::PositionControlInterface
{
public:
    MavlinkPositionControl(uav::UavProtocol *proto, uint8_t id) : m_proto(proto), m_id(id)
    {
    }
    virtual bool goTo(const geo::Coords3D<double> &target) override final;

private:
    uav::UavProtocol *m_proto = nullptr;
    uint8_t m_id              = 0;
};

class MavlinkProtocol : public uav::UavProtocol
{
public:
    explicit MavlinkProtocol();
    ~MavlinkProtocol();

    virtual std::string name() const override;
    virtual std::vector<uint8_t> hello() const override;

    virtual void onReceived(const std::vector<uint8_t> &data) override;

protected:
    virtual void setUAV(int id, uav::UAV *uav) override final;

private:
    void runParser();
    void runMessageReader();

private:
    bool check(char c, mavlink_message_t *msg);

    static std::vector<uint8_t> packMessage(mavlink_message_t *msg);

    const int DIFFERENT_CHANNEL = 1;

    std::queue<mavlink_message_t> _mavlinkMessages;
    std::mutex _mavlinkStoreMutex;

    std::atomic_bool _stopThread;
    std::thread *_dataProcessorThread    = nullptr;
    std::thread *_messageProcessorThread = nullptr;
    std::queue<std::vector<uint8_t>> _dataTasks;
    std::mutex _dataTaskMutex;
};

#endif // MAVLINKPROTOCOL_H
