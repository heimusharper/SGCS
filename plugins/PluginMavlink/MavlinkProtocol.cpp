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
: sgcs::connection::UavProtocol(), DIFFERENT_CHANNEL(1), _bootTime(std::chrono::_V2::system_clock::now())
{
    _stopThread.store(false);
    _dataProcessorThread    = new std::thread(&MavlinkProtocol::runParser, this);
    _messageProcessorThread = new std::thread(&MavlinkProtocol::runMessageReader, this);
    _pingProcessorThread    = new std::thread(&MavlinkProtocol::runPing, this);
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
    if (_pingProcessorThread)
    {
        if (_pingProcessorThread->joinable())
            _pingProcessorThread->join();
        delete _pingProcessorThread;
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
    mavlink_msg_ping_pack_chan(255, 0, DIFFERENT_CHANNEL, &msg, ms.count(), 0, 1, 0);
    MavlinkMessageType t(std::move(msg));
    return t.pack();
}

void MavlinkProtocol::process(const tools::CharMap &data)
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
            for (int i = 0; i < data.size; i++)
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

            if (!_modes.contains(message.sysid))
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
                        MavlinkHelper::ProcessingMode type = MavlinkHelper::mavlinkUavType2SGCS((MAV_TYPE)hrt.type);
                        _modes.insert(std::pair(message.sysid, type));
                        switch (type)
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
                        insertMessage<uav::UAV::Message>(m);
                        break;
                    }
                    default:
                        break;
                }
            }
            else
            {
                MavlinkHelper::ProcessingMode processMode = _modes[message.msgid];

                switch (processMode)
                {
                    case MavlinkHelper::ProcessingMode::UAV_MC:
                    case MavlinkHelper::ProcessingMode::UAV_PLANE:
                    case MavlinkHelper::ProcessingMode::UAV_VTOL:
                    {
                        switch (message.msgid)
                        {
                            case MAVLINK_MSG_ID_HEARTBEAT:
                            {
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
                                insertMessage<uav::AHRS::Message>(ahrs);
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
                                insertMessage<uav::GPS::Message>(gpsm);

                                uav::Position::MessageGPS *pos = new uav::Position::MessageGPS(message.sysid);
                                pos->lat                       = ((double)gps.lat) / 1.E7;
                                pos->lon                       = ((double)gps.lon) / 1.E7;
                                pos->alt                       = ((double)gps.alt) / 1000.;
                                insertMessage<uav::Position::MessageGPS>(pos);
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
                                insertMessage<uav::Home::Message>(home);
                                break;
                            }
                            case MAVLINK_MSG_ID_GPS_STATUS:
                            {
                                mavlink_gps_status_t gps;
                                mavlink_msg_gps_status_decode(&message, &gps);

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
    while (!_stopThread.load())
    {
        BOOST_LOG_TRIVIAL(info) << "PING";
        mavlink_message_t message;
        mavlink_msg_heartbeat_pack(255, 0, &message, MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, 0, 0, 0);
        MavlinkMessageType *msg = new MavlinkMessageType(std::move(message));
        sendMessage(msg);

        usleep(1000000);
    }
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
        mavlink_msg_mission_item_pack_chan(m_id,
                                           0,
                                           0,
                                           &message,
                                           0,
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
        MavlinkMessageType *msg = new MavlinkMessageType(std::move(message));
        m_proto->sendMessage(msg);
        return true;
    }
    return false;
}

tools::CharMap MavlinkMessageType::pack() const
{
    tools::CharMap cm;
    cm.data         = new char[MAVLINK_MAX_PACKET_LEN];
    cm.size         = MAVLINK_MAX_PACKET_LEN;
    uint16_t lenght = mavlink_msg_to_send_buffer((uint8_t *)cm.data, &m_mavlink);
    if (lenght > 0)
    {
        cm.size = lenght;
        return cm;
    }
    return tools::CharMap();
}

mavlink_message_t MavlinkMessageType::mavlink() const
{
    return m_mavlink;
}
