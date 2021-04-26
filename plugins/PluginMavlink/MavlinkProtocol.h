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
#include <GCSStatus.h>
#include <atomic>
#include <boost/log/trivial.hpp>
#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <connection/UavProtocol.h>
#include <map>
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
    bool m_isCheckType = false;

    bool m_waitForHomePoint = true;

    // std::map<int, IAutopilot *> m_modes;
    std::map<int, IAutopilot *> m_modes2;

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
        virtual bool goTo(geo::Coords3D &&target, const geo::Coords3D &base) override final
        {
            m_ap->repositionOffboard(std::move(target), base);
            return false;
        }

    private:
        IAutopilot *m_ap = nullptr;
    };
    class MavlinkRTCMSend : public sgcs::GCSRTK::RTCMCallback
    {
    public:
        MavlinkRTCMSend(IAutopilot *ap) : m_ap(ap)
        {
        }
        virtual void sendRTCM(const tools::CharMap &rtcm) override final
        {
            m_ap->sendRTCM(rtcm);
        }

    private:
        IAutopilot *m_ap = nullptr;
    };
    class MavlinkMissionControl : public uav::Mission::OnChangeMissionCallback
    {
    public:
        MavlinkMissionControl(IAutopilot *ap, uav::UAV *u) : m_ap(ap), m_uav(u)
        {
        }

        virtual void onUpdatePatchs() override final
        {
            // TODO: only first
            if (m_uav->mission()->size() <= 0)
                return;
            const uav::MissionPath &mp = m_uav->mission()->patch(0);
            bool update                = true;
            if (m_representation.contains(mp.uuid()))
                if (m_representation.at(mp.uuid()) == mp.touchUuid())
                    update = false;
            if (update)
                m_ap->doWriteMissionPath(mp, 0);
        }
        virtual void onUpdateItem(int patch, int item) override final
        {
            // TODO: path/item...
        }

    private:
        IAutopilot *m_ap = nullptr;
        uav::UAV *m_uav  = nullptr;

        std::map<boost::uuids::uuid, boost::uuids::uuid> m_representation;
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
                {
                    geo::Coords3D coord;
                    m_uav->position()->gps(coord);
                    geo::Coords3D home;
                    m_uav->home()->position(home);
                    m_ap->repositionOffboard(coord, home);
                    break;
                }
                case uav::UAVControlState::MANUAL_ONBOARD:
                {
                    geo::Coords3D coord;
                    m_uav->position()->gps(coord);
                    geo::Coords3D home;
                    m_uav->home()->position(home);
                    m_ap->repositionOnboard(coord, home);
                    break;
                }
                case uav::UAVControlState::PREPARED:
                    m_ap->requestARM(true, force, false);
                    break;
                case uav::UAVControlState::RTL:
                    m_ap->requestRTL();
                    break;
                case uav::UAVControlState::STARTED:
                {
                    geo::Coords3D coord;
                    m_uav->position()->gps(coord);
                    coord.setAlt(coord.alt() + m_uav->takeoffAltitude());
                    m_ap->requestTakeOff(coord);
                    break;
                }
                case uav::UAVControlState::WAIT:

                    break;
                default:
                    break;
            }
        }

    private:
        IAutopilot *m_ap = nullptr;
        uav::UAV *m_uav  = nullptr;
    };
};

#endif // MAVLINKPROTOCOL_H
