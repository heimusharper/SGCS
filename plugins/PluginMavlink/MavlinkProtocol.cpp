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
    _dataProcessorThread    = new std::thread(&MavlinkProtocol::runParser, this);
    _messageProcessorThread = new std::thread(&MavlinkProtocol::runMessageReader, this);
}

MavlinkProtocol::~MavlinkProtocol()
{
    _stopThread.store(true);
    if (_dataProcessorThread)
    {
        if (_dataProcessorThread->joinable())
            _dataProcessorThread->join();
        delete _dataProcessorThread;
    }
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
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> now =
    std::chrono::_V2::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now - _bootTime);
    mavlink_message_t msg;
    // mavlink_msg_heartbeat_pack_chan(255, 0, DIFFERENT_CHANNEL, &msg, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, 0, 0, MAV_STATE_ACTIVE);
    mavlink_msg_ping_pack_chan(GCS_ID, 0, DIFFERENT_CHANNEL, &msg, ms.count(), 0, 1, 0);
    MavlinkHelper::MavlinkMessageType t =
    MavlinkHelper::MavlinkMessageType(std::move(msg), 1, 0, uav::UavSendMessage::Priority::LOW);
    return t.pack();
}

void MavlinkProtocol::processFromParent(const tools::CharMap &data)
{
    _dataTaskMutex.lock();
    _dataTasks.push(data);
    _dataTaskMutex.unlock();
}

void MavlinkProtocol::processFromChild(const tools::CharMap &data)
{
}

void MavlinkProtocol::runParser()
{
    mavlink_message_t msg;
    while (!_stopThread.load())
    {
        // collect buffers
        while (!_dataTasks.empty())
        {
            _dataTaskMutex.lock();
            tools::CharMap data = _dataTasks.front();
            _dataTasks.pop();
            _dataTaskMutex.unlock();
            for (size_t i = 0; i < data.size; i++)
            {
                if (check(data.data[i], &msg))
                {
                    _mavlinkStoreMutex.lock();
                    _mavlinkMessages.push(msg);
                    _mavlinkStoreMutex.unlock();
                }
            }
        }
        usleep(100);
    }
}

void MavlinkProtocol::runMessageReader()
{
    while (!_stopThread.load())
    {
        while (!_mavlinkMessages.empty())
        {
            _mavlinkStoreMutex.lock();
            mavlink_message_t message = _mavlinkMessages.front();
            _mavlinkMessages.pop();
            _mavlinkStoreMutex.unlock();

            if (!m_modes.contains(message.sysid))
            {
                switch (message.msgid)
                {
                    case MAVLINK_MSG_ID_HEARTBEAT:
                    {
                        uav::UAV::Message *m = new uav::UAV::Message(message.sysid);
                        m->id                = message.sysid;
                        m->type              = uav::UAVType::UNDEFINED;
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
                        m_modes.insert(std::pair(message.sysid, ap));
                        if (ap)
                        {
                            ap->setSend([this](MavlinkHelper::MavlinkMessageType *message) {
                                // BOOST_LOG_TRIVIAL(info) << "WRITE" << message->mavlink().msgid;
                                sendMessage(message);
                            });
                            switch (mode)
                            {
                                case MavlinkHelper::ProcessingMode::ANT:
                                    break;
                                case MavlinkHelper::ProcessingMode::CAMERA:
                                    break;
                                case MavlinkHelper::ProcessingMode::GIMBAL:
                                    break;
                                case MavlinkHelper::ProcessingMode::MODEM:
                                    break;
                                case MavlinkHelper::ProcessingMode::UAV_MC:
                                    m->type = uav::UAVType::MULTICOPTER;
                                    break;
                                case MavlinkHelper::ProcessingMode::UAV_PLANE:
                                    m->type = uav::UAVType::PLANE;
                                    break;
                                case MavlinkHelper::ProcessingMode::UAV_VTOL:
                                    m->type = uav::UAVType::VTOL;
                                    break;
                                case MavlinkHelper::ProcessingMode::UNDEFINED:
                                    break;
                                default:
                                    break;
                            }
                            insertMessage(m);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            //
            BOOST_LOG_TRIVIAL(info) << "IN MESSAGE " << (int)message.sysid << " " << message.msgid;
            if (m_modes.contains(message.sysid))
            {
                IAutopilot *ap = m_modes[message.sysid];
                if (!ap)
                    return;
                // MavlinkHelper::Autopilot ap               = m_modes[message.sysid]->ap;

                switch (ap->processingMode())
                {
                    case MavlinkHelper::ProcessingMode::UAV_MC:
                    case MavlinkHelper::ProcessingMode::UAV_PLANE:
                    case MavlinkHelper::ProcessingMode::UAV_VTOL:
                    {
                        switch (message.msgid)
                        {
                            case MAVLINK_MSG_ID_HEARTBEAT:
                            {
                                if (!ap->ready())
                                {
                                    doConfigure(message.sysid);
                                    ap->setReady(true);
                                }
                                break;
                            }
                            case MAVLINK_MSG_ID_ATTITUDE:
                            {
                                mavlink_attitude_t att;
                                mavlink_msg_attitude_decode(&message, &att);
                                uav::AHRS::Message *ahrs = new uav::AHRS::Message(message.sysid);
                                ahrs->pitch              = static_cast<float>(att.pitch / M_PI * 180.);
                                ahrs->roll               = static_cast<float>(att.roll / M_PI * 180.);
                                ahrs->yaw                = static_cast<float>(att.yaw / M_PI * 180.);
                                insertMessage(ahrs);
                                break;
                            }
                            case MAVLINK_MSG_ID_GPS_RAW_INT:
                            {
                                mavlink_gps_raw_int_t gps;
                                mavlink_msg_gps_raw_int_decode(&message, &gps);
                                uav::GPS::Message *gpsm = new uav::GPS::Message(message.sysid);
                                gpsm->satelitesGPS      = gps.satellites_visible;
                                gpsm->hdop              = gps.h_acc;
                                gpsm->vdop              = gps.v_acc;
                                switch (gps.fix_type)
                                {
                                    case GPS_FIX_TYPE_RTK_FLOAT:
                                    case GPS_FIX_TYPE_RTK_FIXED:
                                        gpsm->fix = uav::GPS::FixType::RTK;
                                        break;
                                    case GPS_FIX_TYPE_3D_FIX:
                                    case GPS_FIX_TYPE_DGPS:
                                        gpsm->fix = uav::GPS::FixType::FIX3D;
                                        break;
                                    case GPS_FIX_TYPE_NO_GPS:
                                    case GPS_FIX_TYPE_NO_FIX:
                                    case GPS_FIX_TYPE_2D_FIX:
                                    case GPS_FIX_TYPE_STATIC:
                                    case GPS_FIX_TYPE_PPP:
                                    default:
                                        gpsm->fix = uav::GPS::FixType::NOGPS;
                                        break;
                                }

                                insertMessage(gpsm);

                                uav::Position::Message *pos = new uav::Position::Message(message.sysid);
                                pos->lat                    = ((double)gps.lat) / 1.E7;
                                pos->lon                    = ((double)gps.lon) / 1.E7;
                                pos->alt                    = ((double)gps.alt) / 1000.;
                                insertMessage(pos);
                                break;
                            }
                            case MAVLINK_MSG_ID_HOME_POSITION:
                            {
                                mavlink_home_position_t pos;
                                mavlink_msg_home_position_decode(&message, &pos);
                                uav::Home::Message *home = new uav::Home::Message(message.sysid);
                                geo::Coords3D coord(((double)pos.latitude) / 1.E7,
                                                    ((double)pos.longitude) / 1.E7,
                                                    ((double)pos.altitude) / 1000.);
                                home->position = coord;
                                insertMessage(home);
                                break;
                            }
                            case MAVLINK_MSG_ID_GPS_STATUS:
                            {
                                mavlink_gps_status_t gps;
                                mavlink_msg_gps_status_decode(&message, &gps);

                                break;
                            }
                            case MAVLINK_MSG_ID_PARAM_VALUE:
                            {
                                mavlink_param_value_t param;
                                mavlink_msg_param_value_decode(&message, &param);
                                std::string id = std::string(param.param_id, strnlen(param.param_id, 16));
                                BOOST_LOG_TRIVIAL(info) << "PARAM ID " << id << " VALUE " << param.param_value;
                                break;
                            }
                            case MAVLINK_MSG_ID_COMMAND_ACK:
                            {
                                mavlink_command_ack_t ack;
                                mavlink_msg_command_ack_decode(&message, &ack);
                                /* switch (ack.result)
                                {
                                    case MAV_RESULT_ACCEPTED:
                                        BOOST_LOG_TRIVIAL(info) << "ACK MESSAGE " << ack.command << " ACCEPTED";
                                        break;
                                    default:
                                        BOOST_LOG_TRIVIAL(info)
                                        << "ACK MESSAGE " << ack.command << " FAILED " << (int)ack.result;
                                        break;
                                } */
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
        usleep(100);
    }
}

void MavlinkProtocol::runPing()
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack(255, 0, &message, MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, 0, 0, 0);
    MavlinkHelper::MavlinkMessageType *msg =
    new MavlinkHelper::MavlinkMessageType(std::move(message), -1, 1000, uav::UavSendMessage::Priority::LOW);
    sendMessage(msg);
}

void MavlinkProtocol::doConfigure(int uav)
{
    doConfigureMessageInterval(uav, IAutopilot::MessageType::ADSB, -1);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::EXTRA1, 1);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::EXTRA2, 1);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::EXTRA3, 1);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::PARAMS, -1);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::POS, 2);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::RAW, -1);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::RC, -1);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::SENSORS, -1);
    doConfigureMessageInterval(uav, IAutopilot::MessageType::STAT, 1);

    runPing();
}

void MavlinkProtocol::doConfigureMessageInterval(int uav, IAutopilot::MessageType msg, int interval_ms)
{
    if (!m_modes.contains(uav))
        return;
    IAutopilot *ap = m_modes[uav];
    if (!ap)
        return;
    ap->setInterval(msg, interval_ms);
}

void MavlinkProtocol::setUAV(int id, uav::UAV *uav)
{
    uav->gps()->setHas(uav::GPS::HAS::HAS_HV_DOP | uav::GPS::HAS::HAS_PROVIDER_GPS);
    uav->position()->setHas(uav::Position::HAS::HAS_SOURCE_GPS);
    MavlinkPositionControl *uavPositionControl = new MavlinkPositionControl(this, 0);
    uav->position()->setControl(uavPositionControl);
    UavProtocol::setUAV(id, uav);
}

bool MavlinkProtocol::check(char c, mavlink_message_t *msg)
{
    mavlink_status_t stats;
    uint8_t i = mavlink_parse_char(DIFFERENT_CHANNEL, c, msg, &stats);
    return i != 0;
}

bool MavlinkPositionControl::goTo(geo::Coords3D &&target)
{
    if (m_proto)
    {
        mavlink_message_t message;
        mavlink_msg_mission_item_pack_chan(255,
                                           0,
                                           0,
                                           &message,
                                           m_id,
                                           0,
                                           0,
                                           MAV_FRAME_GLOBAL_RELATIVE_ALT,
                                           MAV_CMD_NAV_WAYPOINT,
                                           2,
                                           1,
                                           0,
                                           0,
                                           0,
                                           NAN,
                                           (float)target.lat(),
                                           (float)target.lon(),
                                           (float)target.alt(),
                                           MAV_MISSION_TYPE_MISSION);
        MavlinkHelper::MavlinkMessageType *msg =
        new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::HIGHT);
        m_proto->sendMessage(msg);
        return true;
    }
    return false;
}
