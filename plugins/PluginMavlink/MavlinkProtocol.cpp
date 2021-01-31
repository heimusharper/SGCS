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

MavlinkProtocol::MavlinkProtocol(QObject *parent) : uav::UavProtocol(parent)
{
}

QString MavlinkProtocol::name() const
{
    return tr("Mavlink APM");
}

QByteArray MavlinkProtocol::hello() const
{
    mavlink_message_t msg;
    mavlink_msg_ping_pack_chan(0, 0, DIFFERENT_CHANNEL, &msg, 0, 0, 0, 0);
    return packMessage(&msg);
}

void MavlinkProtocol::onReceived(const QByteArray &data)
{
    mavlink_message_t msg;
    for (int i = 0; i < data.size(); i++)
    {
        if (check(data.at(i), &msg))
        {
            qInfo() << "MSG ID" << msg.msgid;
            if (_waitForSignal)
            {
                _waitForSignal = false;
                emit onReadyData();
            }
        }
    }
}

bool MavlinkProtocol::check(char c, mavlink_message_t *msg)
{
    mavlink_status_t stats;
    uint8_t i = mavlink_parse_char(DIFFERENT_CHANNEL, c, msg, &stats);
    if (i != 0)
    {
        if (!_isCheckAPM)
        {
            if (msg->msgid == MAVLINK_MSG_ID_HEARTBEAT)
            {
                mavlink_heartbeat_t hrt;
                mavlink_msg_heartbeat_decode(msg, &hrt);
                if (hrt.autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA)
                {
                    _isCheckAPM = true;
                    return true;
                }
            }
            return false;
        }
        return true;
    }
    return false;
}

QByteArray MavlinkProtocol::packMessage(mavlink_message_t *msg)
{
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    int lenght = mavlink_msg_to_send_buffer(buffer, msg);
    if (lenght > 0)
    {
        return QByteArray((const char *)buffer, lenght);
    }
    return QByteArray();
}
