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

class MavlinkProtocol : public sgcs::connection::UavProtocol
{
public:
    explicit MavlinkProtocol();
    ~MavlinkProtocol();

    virtual std::string name() const override;
    virtual tools::CharMap hello() const override;

    virtual void processFromChild(const tools::CharMap &data) override final;

protected:
    virtual void setUAV(int id, uav::UAV *uav) override final;

    virtual std::list<uav::UavSendMessage *> createUAVMesage(const tools::CharMap &data)
    {
        mavlink_message_t msg;
        std::list<uav::UavSendMessage *> messages;
        for (size_t i = 0; i < data.size; i++)
        {
            if (check(data.data[i], &msg))
            {
                mavlink_message_t mcopy;
                memcpy(&mcopy, &msg, sizeof(mavlink_message_t));
                messages.push_back(new MavlinkHelper::MavlinkMessageType(std::move(mcopy)));
                _mavlinkStoreMutex.lock();
                _mavlinkMessages.push_back(new MavlinkHelper::MavlinkMessageType(std::move(msg)));
                _mavlinkStoreMutex.unlock();
            }
        }
        return messages;
    }

private:
    void runMessageReader();
    void runPing();

    void doConfigure(int uav);

    void doConfigureMessageInterval(int uav, IAutopilot::MessageType msg, int interval_ms);

private:
    bool check(char c, mavlink_message_t *msg);

    const int DIFFERENT_CHANNEL;
    const uint8_t GCS_ID;
    const std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> _bootTime;

    std::vector<uav::UavSendMessage *> _mavlinkMessages;
    std::mutex _mavlinkStoreMutex;

    std::atomic_bool _stopThread;
    std::thread *_messageProcessorThread = nullptr;
    std::mutex _dataTaskMutex;

    std::map<int, IAutopilot *> m_modes;

private:
    class MavlinkAHRSControl : public uav::AHRS::OnChangeAHRSCallback
    {
    public:
        MavlinkAHRSControl(IAutopilot *ap) : m_ap(ap)
        {
        }
        virtual void sendYaw(float yaw)
        {
            m_ap->repositionAzimuth(yaw);
        }

    private:
        IAutopilot *m_ap = nullptr;
    };

    class MavlinkSpeedControl : public uav::Speed::OnChangeSpeedCallback
    {
    public:
        MavlinkSpeedControl(IAutopilot *ap) : m_ap(ap)
        {
        }
        virtual void sendSpeed(float newSpeed) override final
        {
            m_ap->sendSpeed(newSpeed);
        }

    private:
        IAutopilot *m_ap = nullptr;
    };

    class MavlinkPositionControl : public uav::Position::PositionControlInterface
    {
    public:
        MavlinkPositionControl(IAutopilot *ap) : m_ap(ap)
        {
        }
        virtual bool goTo(geo::Coords3D &&target) override final
        {
            m_ap->repositionOffboard(std::move(target));
            return false;
        }

    private:
        IAutopilot *m_ap = nullptr;
    };

    class MavlinkARMControl : public uav::UAV::ControlInterface
    {
    public:
        MavlinkARMControl(IAutopilot *ap, uav::UAV *u) : m_ap(ap), m_uav(u)
        {
        }
        virtual void changeState(uav::UAVControlState state, bool force = false) override final
        {
            switch (state)
            {
                case uav::UAVControlState::AUTO:

                    break;
                case uav::UAVControlState::KILL:
                    m_ap->requestDisARM(force);
                    break;
                case uav::UAVControlState::LAND:
                    m_ap->requestLand();
                    break;
                case uav::UAVControlState::MANUAL_OFFBOARD:
                    m_ap->repositionOffboard(m_uav->position()->gps());
                    break;
                case uav::UAVControlState::MANUAL_ONBOARD:
                    m_ap->repositionOnboard(m_uav->position()->gps());
                    break;
                case uav::UAVControlState::PREPARED:
                    m_ap->requestARM(true, force, false);
                    break;
                case uav::UAVControlState::RTL:

                    break;
                case uav::UAVControlState::STARTED:
                    m_ap->requestTakeOff(altitude);
                    break;
                case uav::UAVControlState::WAIT:

                    break;
                default:
                    break;
            }
        }

    private:
        uav::UAV *m_uav  = nullptr;
        IAutopilot *m_ap = nullptr;
        float altitude   = 10;
    };
};

#endif // MAVLINKPROTOCOL_H
