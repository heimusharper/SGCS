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
#include "AutopilotAPMImpl.h"
#include "AutopilotPixhawkImpl.h"
#include "IAutopilot.h"
#include "MavlinkConfig.h"
#include "MavlinkHelper.h"
#include <atomic>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <connection/UavProtocol.h>
#include <queue>
#include <thread>

class MavlinkPositionControl : public uav::Position::PositionControlInterface
{
public:
    MavlinkPositionControl(sgcs::connection::UavProtocol *proto, uint8_t id) : m_proto(proto), m_id(id)
    {
    }
    virtual bool goTo(geo::Coords3D &&target) override final;

private:
    sgcs::connection::UavProtocol *m_proto = nullptr;
    uint8_t m_id                           = 0;
};

class MavlinkProtocol : public sgcs::connection::UavProtocol
{
public:
    explicit MavlinkProtocol();
    ~MavlinkProtocol();

    virtual std::string name() const override;
    virtual tools::CharMap hello() const override;

    virtual void processFromChild(const tools::CharMap &data) override final;
    virtual void processFromParent(const tools::CharMap &data) override final;

protected:
    virtual void setUAV(int id, uav::UAV *uav) override final;

private:
    void runParser();
    void runMessageReader();
    void runPing();

    void doConfigure(int uav);

    void doConfigureMessageInterval(int uav, IAutopilot::MessageType msg, int interval_ms);

private:
    bool check(char c, mavlink_message_t *msg);

    const int DIFFERENT_CHANNEL;
    const uint8_t GCS_ID;
    const std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> _bootTime;

    std::queue<mavlink_message_t> _mavlinkMessages;
    std::mutex _mavlinkStoreMutex;

    std::atomic_bool _stopThread;
    std::thread *_dataProcessorThread    = nullptr;
    std::thread *_messageProcessorThread = nullptr;
    std::queue<tools::CharMap> _dataTasks;
    std::mutex _dataTaskMutex;

    std::map<int, IAutopilot *> m_modes;
};

#endif // MAVLINKPROTOCOL_H
