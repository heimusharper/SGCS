#include "AutopilotPixhawkImpl.h"

AutopilotPixhawkImpl::AutopilotPixhawkImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: IAutopilot(chan, gcsID, id, mode)
{
}

bool AutopilotPixhawkImpl::setInterval(IAutopilot::MessageType type, int interval_hz)
{
    std::list<int> msg_ids;
    switch (type)
    {
        case MessageType::ADSB:
            // msg_ids.push_back(MAVLINK_MSG_ID_ADSB);
            break;
        case MessageType::EXTRA1:
            msg_ids.push_back(MAVLINK_MSG_ID_ATTITUDE_TARGET);
            msg_ids.push_back(MAVLINK_MSG_ID_AHRS2);
            msg_ids.push_back(MAVLINK_MSG_ID_PID_TUNING);
            break;
        case MessageType::EXTRA2:
            msg_ids.push_back(MAVLINK_MSG_ID_TIMESYNC);
            msg_ids.push_back(MAVLINK_MSG_ID_EXTENDED_SYS_STATE);
            msg_ids.push_back(MAVLINK_MSG_ID_VFR_HUD);
            break;
        case MessageType::EXTRA3:
            msg_ids.push_back(MAVLINK_MSG_ID_AHRS);
            msg_ids.push_back(MAVLINK_MSG_ID_SYSTEM_TIME);
            msg_ids.push_back(MAVLINK_MSG_ID_HWSTATUS);
            msg_ids.push_back(MAVLINK_MSG_ID_RANGEFINDER);
            msg_ids.push_back(MAVLINK_MSG_ID_DISTANCE_SENSOR);
            msg_ids.push_back(MAVLINK_MSG_ID_TERRAIN_REQUEST);
            msg_ids.push_back(MAVLINK_MSG_ID_BATTERY2);
            msg_ids.push_back(MAVLINK_MSG_ID_MOUNT_STATUS);
            msg_ids.push_back(MAVLINK_MSG_ID_OPTICAL_FLOW);
            msg_ids.push_back(MAVLINK_MSG_ID_GIMBAL_REPORT);
            msg_ids.push_back(MAVLINK_MSG_ID_MAG_CAL_REPORT);
            msg_ids.push_back(MAVLINK_MSG_ID_EKF_STATUS_REPORT);
            msg_ids.push_back(MAVLINK_MSG_ID_VIBRATION);
            msg_ids.push_back(MAVLINK_MSG_ID_RPM);
            msg_ids.push_back(MAVLINK_MSG_ID_HIGHRES_IMU);
            break;
        case MessageType::PARAMS:
            msg_ids.push_back(MAVLINK_MSG_ID_PARAM_VALUE);
            break;
        case MessageType::POS:
            msg_ids.push_back(MAVLINK_MSG_ID_ALTITUDE);
            msg_ids.push_back(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
            msg_ids.push_back(MAVLINK_MSG_ID_POSITION_TARGET_GLOBAL_INT);
            break;
        case MessageType::RAW:
            msg_ids.push_back(MAVLINK_MSG_ID_RAW_IMU);
            msg_ids.push_back(MAVLINK_MSG_ID_SCALED_IMU);
            msg_ids.push_back(MAVLINK_MSG_ID_SCALED_IMU2);
            msg_ids.push_back(MAVLINK_MSG_ID_SCALED_IMU3);
            msg_ids.push_back(MAVLINK_MSG_ID_SCALED_PRESSURE);
            msg_ids.push_back(MAVLINK_MSG_ID_SCALED_PRESSURE2);
            msg_ids.push_back(MAVLINK_MSG_ID_SCALED_PRESSURE3);
            msg_ids.push_back(MAVLINK_MSG_ID_ACTUATOR_CONTROL_TARGET);
            break;
        case MessageType::RC:
            msg_ids.push_back(MAVLINK_MSG_ID_SERVO_OUTPUT_RAW);
            break;
        case MessageType::SENSORS:
            msg_ids.push_back(MAVLINK_MSG_ID_RC_CHANNELS_SCALED);
            msg_ids.push_back(MAVLINK_MSG_ID_BATTERY_STATUS);
            break;
        case MessageType::STAT:
            msg_ids.push_back(MAVLINK_MSG_ID_SYS_STATUS);
            msg_ids.push_back(MAVLINK_MSG_ID_POWER_STATUS);
            msg_ids.push_back(MAVLINK_MSG_ID_GPS_RAW_INT);
            msg_ids.push_back(MAVLINK_MSG_ID_GPS_RTK);
            msg_ids.push_back(MAVLINK_MSG_ID_GPS2_RAW);
            msg_ids.push_back(MAVLINK_MSG_ID_GPS2_RTK);
            msg_ids.push_back(MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT);
            msg_ids.push_back(MAVLINK_MSG_ID_FENCE_STATUS);
            break;
        default:
            break;
    }
    if (msg_ids.empty())
        return false;
    for (const int &i : msg_ids)
    {
        mavlink_message_t message;
        mavlink_msg_command_long_pack_chan(m_gcs,
                                           0,
                                           m_chanel,
                                           &message,
                                           m_id,
                                           0,
                                           MAV_CMD_SET_MESSAGE_INTERVAL,
                                           1,
                                           i,
                                           (interval_hz < 0) ? -1 : (int)(1000000.f / (float)interval_hz),
                                           0,
                                           0,
                                           0,
                                           0,
                                           0);

        // mavlink_msg_request_data_stream_pack_chan(
        // m_gcs, 0, m_chanel, &message, m_id, 0, i, (interval_hz < 0) ? 0 : interval_hz, (interval_hz < 0) ? 0 : 1);
        m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::LOW));

        //  BOOST_LOG_TRIVIAL(info) << "CONSTRUCT RATE " << m_gcs << " " << m_chanel << " " << m_id << " " << interval_hz << " " << i;
    }
    return true;
}
