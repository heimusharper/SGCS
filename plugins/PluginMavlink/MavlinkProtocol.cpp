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

MavlinkProtocol::MavlinkProtocol() : uav::UavProtocol()
{
    _isCheckAPM.store(false);
    _waitForSignal.store(true);
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
    return "Mavlink APM";
}

std::vector<uint8_t> MavlinkProtocol::hello() const
{
    mavlink_message_t msg;
    mavlink_msg_ping_pack_chan(0, 0, DIFFERENT_CHANNEL, &msg, 0, 0, 0, 0);
    return packMessage(&msg);
}

void MavlinkProtocol::onReceived(const std::vector<uint8_t> &data)
{
    _dataTaskMutex.lock();
    _dataTasks.push(data);
    _dataTaskMutex.unlock();
}

void MavlinkProtocol::runParser()
{
    std::vector<uint8_t> buffer;
    while (!_stopThread.load())
    {
        // collect buffers
        while (!_dataTasks.empty())
        {
            _dataTaskMutex.lock();
            std::vector<uint8_t> data = _dataTasks.front();
            _dataTasks.pop();
            _dataTaskMutex.unlock();
            for (int i = 0; i < data.size(); i++)
                buffer.push_back(data.at(i));
        }

        // parser
        if (!buffer.empty())
        {
            mavlink_message_t msg;
            for (int i = 0; i < buffer.size(); i++)
            {
                if (check((char)buffer[i], &msg))
                {
                    if (_isCheckAPM.load() && msg.sysid == m_uavID)
                    {
                        _mavlinkStoreMutex.lock();
                        _mavlinkMessages.push(msg);
                        _mavlinkStoreMutex.unlock();
                        // BOOST_LOG_TRIVIAL(info) << "MSG ID" << msg.msgid;
                        if (_waitForSignal.load())
                        {
                            _waitForSignal.store(false);
                        }
                    }
                }
            }
            buffer.clear();
        }

        usleep(10000);
    }
}

void MavlinkProtocol::runMessageReader()
{
    bool isSetID = false;
    while (!_stopThread.load())
    {
        while (!_mavlinkMessages.empty())
        {
            _mavlinkStoreMutex.lock();
            mavlink_message_t message = _mavlinkMessages.front();
            _mavlinkMessages.pop();
            _mavlinkStoreMutex.unlock();
            // BOOST_LOG_TRIVIAL(info) << "MSG ID" << message.msgid;

            switch (message.msgid)
            {
                case MAVLINK_MSG_ID_HEARTBEAT:
                {
                    if (!isSetID)
                    {
                        isSetID              = true;
                        uav::UAV::Message *m = new uav::UAV::Message();
                        m->id                = message.sysid;
                        insertMessage<uav::UAV::Message>(m);
                    }
                    break;
                }
                case MAVLINK_MSG_ID_ATTITUDE:
                {
                    mavlink_attitude_t att;
                    mavlink_msg_attitude_decode(&message, &att);
                    uav::AHRS::Message *ahrs = new uav::AHRS::Message();
                    ahrs->pitch              = static_cast<float>(att.pitch / M_PI * 180.);
                    ahrs->roll               = static_cast<float>(att.roll / M_PI * 180.);
                    ahrs->yaw                = static_cast<float>(att.yaw / M_PI * 180.);
                    insertMessage<uav::AHRS::Message>(ahrs);
                    break;
                }
                default:
                    break;
            }
        }
        usleep(10000);
    }
}

void MavlinkProtocol::onSetUAV()
{
}

bool MavlinkProtocol::check(char c, mavlink_message_t *msg)
{
    mavlink_status_t stats;
    uint8_t i = mavlink_parse_char(DIFFERENT_CHANNEL, c, msg, &stats);
    if (i != 0)
    {
        if (!_isCheckAPM.load())
        {
            if (msg->msgid == MAVLINK_MSG_ID_HEARTBEAT)
            {
                mavlink_heartbeat_t hrt;
                mavlink_msg_heartbeat_decode(msg, &hrt);
                if (hrt.autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA)
                {
                    m_uavID = msg->sysid;
                    _isCheckAPM.store(true);
                    return true;
                }
            }
            return false;
        }
        return true;
    }
    return false;
}

std::vector<uint8_t> MavlinkProtocol::packMessage(mavlink_message_t *msg)
{
    std::vector<uint8_t> data = std::vector<uint8_t>(MAVLINK_MAX_PACKET_LEN, (uint8_t)0x0);
    int lenght                = mavlink_msg_to_send_buffer(data.data(), msg);
    if (lenght > 0)
    {
        while (data.size() > lenght)
            data.pop_back();
        return data;
    }
    return std::vector<uint8_t>();
}
