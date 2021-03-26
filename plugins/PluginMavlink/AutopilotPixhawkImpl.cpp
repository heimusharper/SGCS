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
            msg_ids.push_back(MAVLINK_MSG_ID_ATTITUDE);
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
            msg_ids.push_back(MAVLINK_MSG_ID_LOCAL_POSITION_NED);
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
            msg_ids.push_back(MAVLINK_MSG_ID_ODOMETRY);
            msg_ids.push_back(MAVLINK_MSG_ID_ESTIMATOR_STATUS);
            msg_ids.push_back(MAVLINK_MSG_ID_ATTITUDE_QUATERNION);
            msg_ids.push_back(MAVLINK_MSG_ID_ATTITUDE_TARGET);
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
        m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::LOW));
    }
    return true;
}

bool AutopilotPixhawkImpl::requestARM(bool autoChangeMode, bool force, bool defaultModeAuto)
{
    BOOST_LOG_TRIVIAL(info) << "DO ARM";
    bool readyToStart = false;
    if (m_baseMode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED)
    {
        union px4::px4_custom_mode px4_mode;
        px4_mode.data = m_customMode;
        if (px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_AUTO || px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD)
            readyToStart = true;
        if (readyToStart)
        {
            // arm
            arm(force);
            return true;
        }
        else if (autoChangeMode)
        {
            // mode change
            union px4::px4_custom_mode px4_mode;
            if (defaultModeAuto)
            {
                px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
                px4_mode.sub_mode  = px4::PX4_CUSTOM_SUB_MODE_AUTO_READY;
            }
            else
            {
                px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
                px4_mode.sub_mode  = 0;
            }
            target_main_mode   = px4_mode.main_mode;
            target_sub_mode    = px4_mode.sub_mode;
            target_force_arm   = force;
            m_waitPrepareToARM = 5;
            sendMode(0, px4_mode.data);
            return true;
        }
    }
    return false;
}

bool AutopilotPixhawkImpl::requestDisARM(bool force)
{
    BOOST_LOG_TRIVIAL(info) << "DO DISARM";
    disarm(force);
}

bool AutopilotPixhawkImpl::requestTakeOff(int altitude)
{
    BOOST_LOG_TRIVIAL(info) << "DO TAKEOFF";
    mavlink_message_t message;
    union px4::px4_custom_mode px4_mode;
    px4_mode.data = m_customMode;
    if (px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_AUTO)
        mavlink_msg_command_long_pack_chan(m_gcs, 0, m_chanel, &message, m_id, 0, MAV_CMD_MISSION_START, 1, 0, 0, 0, 0, 0, 0, 0);
    else if (px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD)
        mavlink_msg_command_long_pack_chan(m_gcs, 0, m_chanel, &message, m_id, 0, MAV_CMD_NAV_TAKEOFF, 1, 0, altitude, 0, 0, 0, 0, 0);
    else
        return false;
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::HIGHT));
    return true;
}

uav::UAVControlState AutopilotPixhawkImpl::getState(bool &done) const
{
    done = true;
    if (!isFlight)
    {
        return uav::UAVControlState::WAIT;
    }
    else
    {
        union px4::px4_custom_mode px4_mode;
        px4_mode.data = m_customMode;
        switch (px4_mode.main_mode)
        {
            case px4::PX4_CUSTOM_MAIN_MODE_MANUAL:
            case px4::PX4_CUSTOM_MAIN_MODE_ALTCTL:
            case px4::PX4_CUSTOM_MAIN_MODE_POSCTL:
            case px4::PX4_CUSTOM_MAIN_MODE_ACRO:
            case px4::PX4_CUSTOM_MAIN_MODE_STABILIZED:
            case px4::PX4_CUSTOM_MAIN_MODE_RATTITUDE:
                return uav::UAVControlState::MANUAL_ONBOARD;
            case px4::PX4_CUSTOM_MAIN_MODE_AUTO:
            {
                switch (px4_mode.sub_mode)
                {
                    case px4::PX4_CUSTOM_SUB_MODE_AUTO_READY:
                        return uav::UAVControlState::PREPARED;
                    case px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF:
                        return uav::UAVControlState::STARTED;
                    case px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER:
                    case px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION:
                        return uav::UAVControlState::AUTO;
                    case px4::PX4_CUSTOM_SUB_MODE_AUTO_RTGS:
                    case px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET:
                    case px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL:
                        return uav::UAVControlState::RTL;
                    case px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND:
                        return uav::UAVControlState::LAND;
                        break;
                    default:
                        break;
                }
                return uav::UAVControlState::AUTO;
            }
            case px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD:
            default:
                return uav::UAVControlState::MANUAL_OFFBOARD;
        }
    }
    done = false;
}

bool AutopilotPixhawkImpl::repositionOnboard(geo::Coords3D &&pos)
{
    BOOST_LOG_TRIVIAL(info) << "DO REPOSITION" << pos.lat() << ";" << pos.lon() << ";" << pos.alt();
    mavlink_message_t message;
    mavlink_msg_mission_item_pack_chan(m_gcs,
                                       0,
                                       m_chanel,
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
                                       (float)pos.lat(),
                                       (float)pos.lon(),
                                       (float)pos.alt(),
                                       MAV_MISSION_TYPE_MISSION);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 2, 200, uav::UavSendMessage::Priority::HIGHT));
    return true;
}

bool AutopilotPixhawkImpl::repositionOffboard(geo::Coords3D &&pos)
{
    return repositionOnboard(std::move(pos));
}

void AutopilotPixhawkImpl::setMode(uint8_t base, uint32_t custom)
{
    BOOST_LOG_TRIVIAL(info) << "DO MODE " << (int)base << " " << custom;
    IAutopilot::setMode(base, custom);
    if (m_waitPrepareToARM-- > 0)
    {
        union px4::px4_custom_mode px4_mode;
        px4_mode.data = custom;
        if (target_main_mode == px4_mode.main_mode && target_sub_mode == px4_mode.sub_mode)
        {
            requestARM(false, target_force_arm);
        }
    }
}
