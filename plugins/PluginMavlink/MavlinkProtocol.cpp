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
#include "MavlinkProtocol.h"

MavlinkProtocol::MavlinkProtocol()
: sgcs::connection::UavProtocol(), DIFFERENT_CHANNEL(0), GCS_ID(255), _bootTime(std::chrono::_V2::system_clock::now())
{
    _stopThread.store(false);
    _messageProcessorThread = new std::thread(&MavlinkProtocol::runMessageReader, this);
}

MavlinkProtocol::~MavlinkProtocol()
{
    _stopThread.store(true);
    if (_messageProcessorThread)
    {
        if (_messageProcessorThread->joinable())
            _messageProcessorThread->join();
        delete _messageProcessorThread;
    }
}

std::string MavlinkProtocol::name() const
{
    return "Mavlink";
}

tools::CharMap MavlinkProtocol::hello() const
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now - _bootTime);
    mavlink_message_t msg;
    // mavlink_msg_heartbeat_pack_chan(255, 0, DIFFERENT_CHANNEL, &msg, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, 0, 0, MAV_STATE_ACTIVE);
    mavlink_msg_ping_pack_chan(GCS_ID, 0, DIFFERENT_CHANNEL, &msg, ms.count(), 0, 1, 0);
    MavlinkHelper::MavlinkMessageType t =
    MavlinkHelper::MavlinkMessageType(std::move(msg), 1, 0, uav::UavSendMessage::Priority::LOW);
    return t.pack();
}

void MavlinkProtocol::processFromChild(const tools::CharMap &data)
{
}

void MavlinkProtocol::runMessageReader()
{
    while (!_stopThread.load())
    {
        while (true)
        {
            uav::UavSendMessage *messageref = nullptr;
            {
                std::lock_guard grd(_mavlinkStoreMutex);
                if (_mavlinkMessages.empty())
                    break;
                messageref = _mavlinkMessages.back();
                _mavlinkMessages.pop_back();
            }
            mavlink_message_t message = dynamic_cast<MavlinkHelper::MavlinkMessageType *>(messageref)->mavlink();
            delete messageref; // clear
            if (!m_modes2.contains(message.sysid))
            {
                switch (message.msgid)
                {
                    case MAVLINK_MSG_ID_HEARTBEAT:
                    {
                        int id = message.sysid;

                        mavlink_heartbeat_t hrt;
                        mavlink_msg_heartbeat_decode(&message, &hrt);

                        IAutopilot *ap = nullptr;
                        auto mode      = MavlinkHelper::mavlinkUavType2SGCS((MAV_TYPE)hrt.type);
                        switch (hrt.autopilot)
                        {
                            case MAV_AUTOPILOT_ARDUPILOTMEGA:
                                ap = new AutopilotAPMImpl(DIFFERENT_CHANNEL, GCS_ID, message.sysid, mode);
                                break;
                            case MAV_AUTOPILOT_INVALID:
                                break;
                            case MAV_AUTOPILOT_GENERIC:
                            case MAV_AUTOPILOT_RESERVED:
                            case MAV_AUTOPILOT_SLUGS:
                            case MAV_AUTOPILOT_OPENPILOT:
                            case MAV_AUTOPILOT_GENERIC_WAYPOINTS_ONLY:
                            case MAV_AUTOPILOT_GENERIC_WAYPOINTS_AND_SIMPLE_NAVIGATION_ONLY:
                            case MAV_AUTOPILOT_GENERIC_MISSION_FULL:
                            case MAV_AUTOPILOT_PPZ:
                            case MAV_AUTOPILOT_UDB:
                            case MAV_AUTOPILOT_FP:
                            case MAV_AUTOPILOT_PX4:
                            case MAV_AUTOPILOT_SMACCMPILOT:
                            case MAV_AUTOPILOT_AUTOQUAD:
                            case MAV_AUTOPILOT_ARMAZILA:
                            case MAV_AUTOPILOT_AEROB:
                            case MAV_AUTOPILOT_ASLUAV:
                            case MAV_AUTOPILOT_SMARTAP:
                            case MAV_AUTOPILOT_AIRRAILS:
                            default:
                                ap = new AutopilotPixhawkImpl(DIFFERENT_CHANNEL, GCS_ID, message.sysid, mode);
                        }
                        if (ap)
                        {
                            // m_modes.insert(std::make_pair(message.sysid, ap));
                            m_modes2.insert(std::make_pair(message.sysid, ap));

                            ap->setRemove([this](int id) {
                                m_sendMutex.lock();
                                auto rmx = [id](std::vector<uav::UavSendMessage *> *msgs) {
                                    auto x = std::remove_if(msgs->begin(), msgs->end(), [id](const uav::UavSendMessage *obj) {
                                        const MavlinkHelper::MavlinkMessageType *d =
                                        dynamic_cast<const MavlinkHelper::MavlinkMessageType *>(obj);
                                        if (d)
                                            return d->mavlink().msgid == id;
                                        return false;
                                    });
                                    if (x != msgs->end())
                                        msgs->erase(x);
                                };
                                rmx(m_send[uav::UavSendMessage::Priority::HIGHT]);
                                rmx(m_send[uav::UavSendMessage::Priority::LOW]);
                                rmx(m_send[uav::UavSendMessage::Priority::NORMAL]);
                                m_sendMutex.unlock();
                            });
                            ap->setSend([this](MavlinkHelper::MavlinkMessageTypeI *message) {
                                // BOOST_LOG_TRIVIAL(info) << "WRITE" << message->mavlink().msgid;
                                sendMessage(message);
                            });
                            checkUAV(id);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            if (m_modes2.contains(message.sysid))
            {
                IAutopilot *ap = m_modes2.at(message.sysid);
                if (!ap || !ap->getUav())
                    continue;
                switch (ap->processingMode())
                {
                    case MavlinkHelper::ProcessingMode::UAV_MC:
                    case MavlinkHelper::ProcessingMode::UAV_PLANE:
                    case MavlinkHelper::ProcessingMode::UAV_VTOL:
                    {
                        switch (message.msgid)
                        {
                            case MAVLINK_MSG_ID_PING:
                            {
                                /*mavlink_ping_t time;
                                mavlink_msg_ping_decode(&message, &time);
                                ap->setBootTimeMS(time.time_usec);*/
                                break;
                            }
                            case MAVLINK_MSG_ID_HEARTBEAT:
                            {
                                mavlink_heartbeat_t hrt;
                                mavlink_msg_heartbeat_decode(&message, &hrt);
                                ap->setHeartbeat(hrt);

                                break;
                            }
                            case MAVLINK_MSG_ID_ATTITUDE:
                            {
                                mavlink_attitude_t att;
                                mavlink_msg_attitude_decode(&message, &att);
                                ap->setAttitudes(att);
                                break;
                            }
                            case MAVLINK_MSG_ID_GPS_RAW_INT:
                            {
                                // BOOST_LOG_TRIVIAL(info) << "MAVLINK_MSG_ID_GPS_RAW_INT";
                                mavlink_gps_raw_int_t gps;
                                mavlink_msg_gps_raw_int_decode(&message, &gps);
                                ap->setGPSRawInt(gps);
                                break;
                            }
                            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
                            {
                                mavlink_global_position_int_t gps;
                                mavlink_msg_global_position_int_decode(&message, &gps);
                                ap->setGlobalPositionInt(gps);
                                break;
                            }
                            case MAVLINK_MSG_ID_SYS_STATUS:
                            {
                                mavlink_sys_status_t stat;
                                mavlink_msg_sys_status_decode(&message, &stat);
                                ap->setSysStatus(stat);
                                break;
                            }
                            case MAVLINK_MSG_ID_POSITION_TARGET_GLOBAL_INT:
                            {
                                mavlink_position_target_global_int_t pt;
                                mavlink_msg_position_target_global_int_decode(&message, &pt);
                                break;
                            }
                            case MAVLINK_MSG_ID_HOME_POSITION:
                            {
                                mavlink_home_position_t pos;
                                mavlink_msg_home_position_decode(&message, &pos);
                                ap->setHomePosition(pos);
                                break;
                            }
                            case MAVLINK_MSG_ID_VFR_HUD:
                            {
                                mavlink_vfr_hud_t hud;
                                mavlink_msg_vfr_hud_decode(&message, &hud);
                                ap->setVFRHud(hud);
                                break;
                            }
                            case MAVLINK_MSG_ID_GPS_STATUS:
                            {
                                // BOOST_LOG_TRIVIAL(info) << "MAVLINK_MSG_ID_GPS_STATUS";
                                mavlink_gps_status_t gps;
                                mavlink_msg_gps_status_decode(&message, &gps);
                                break;
                            }
                            case MAVLINK_MSG_ID_PARAM_VALUE:
                            {
                                mavlink_param_value_t param;
                                mavlink_msg_param_value_decode(&message, &param);
                                std::string data = std::string(param.param_id, strnlen(param.param_id, 15));
                                ap->setParamValue(data, param.param_value);
                                // std::string id = std::string(param.param_id, strnlen(param.param_id, 16));
                                // BOOST_LOG_TRIVIAL(info) << "PARAM ID " << id << " VALUE " << param.param_value;
                                break;
                            }
                            case MAVLINK_MSG_ID_COMMAND_ACK:
                            {
                                mavlink_command_ack_t ack;
                                mavlink_msg_command_ack_decode(&message, &ack);
                                ap->setAck(ack);
                                break;
                            }
                            case MAVLINK_MSG_ID_SYSTEM_TIME:
                            {
                                mavlink_system_time_t time;
                                mavlink_msg_system_time_decode(&message, &time);
                                ap->setSystemTime(time);
                                break;
                            }
                            case MAVLINK_MSG_ID_STATUSTEXT:
                            {
                                mavlink_statustext_t text;
                                mavlink_msg_statustext_decode(&message, &text);
                                std::string data = std::string(text.text, strnlen(text.text, 50));
                                ap->setStatusText(data);
                                BOOST_LOG_TRIVIAL(warning) << "STATUS_TEXT " << data;

                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        usleep(1000);
    }
}

void MavlinkProtocol::setUAV(int id, uav::UAV *uav)
{
    uav->gps()->setHas(uav::GPS::HAS::HAS_HV_DOP | uav::GPS::HAS::HAS_PROVIDER_GPS);
    uav->position()->setHas(uav::Position::HAS::HAS_SOURCE_GPS);
    uav->speed()->setHas(uav::Speed::HAS_GROUND_SPEED);
    uav->status()->setHas(uav::Status::MAG | uav::Status::GPS | uav::Status::ACCEL);
    uav->calibration()->setHas(uav::Calibration::HAS_MAGNETOMETER);

    MavlinkPositionControl *uavPositionControl = new MavlinkPositionControl(m_modes2.at(id));
    MavlinkAHRSControl *ahrsPositionControl    = new MavlinkAHRSControl(m_modes2.at(id));
    MavlinkSpeedControl *speedControl          = new MavlinkSpeedControl(m_modes2.at(id));
    MavlinkARMControl *armControl              = new MavlinkARMControl(m_modes2.at(id));
    MavlinkRTCMSend *rtcmSend                  = new MavlinkRTCMSend(m_modes2.at(id));
    MavlinkCalibrationControl *cal             = new MavlinkCalibrationControl(m_modes2.at(id));

    uav->position()->setControl(uavPositionControl);
    uav->ahrs()->addCallback(ahrsPositionControl);
    uav->speed()->addCallback(speedControl);
    uav->addControl(armControl);
    uav->calibration()->addControl(cal);
    sgcs::GCSStatus::instance().rtk()->addCallback(rtcmSend);

    uav->setTakeoffAltitude(10);
    uav->mission()->setMaxPatchsCount(1);
    m_modes2.at(id)->setUav(uav);
    UavProtocol::setUAV(id, uav);
}

bool MavlinkProtocol::check(char c, mavlink_message_t *msg)
{
    mavlink_status_t stats;
    uint8_t i = mavlink_parse_char(DIFFERENT_CHANNEL, c, msg, &stats);
    return i != 0;
}
