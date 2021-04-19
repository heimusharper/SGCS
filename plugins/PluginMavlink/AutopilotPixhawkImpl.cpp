#include "AutopilotPixhawkImpl.h"

AutopilotPixhawkImpl::AutopilotPixhawkImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: IAutopilot(chan, gcsID, id, mode)
, m_waitPrepareToARM()
, m_waitPrepareToARMTimer(std::chrono::system_clock::now())
, m_waitForRepositionOFFBOARD(false)
, m_target_main_mode(0)
, m_target_sub_mode(0)
, target_force_arm(false)
, m_lastRepositionPos(geo::Coords3D())
, m_lastYaw(NAN)
{
    m_repositionThreadWorks.store(false);
    m_repositionThreadStop.store(false);
    m_repositionThread = new std::thread(&AutopilotPixhawkImpl::doRepositionTick, this);
}

AutopilotPixhawkImpl::~AutopilotPixhawkImpl()
{
    if (m_repositionThread)
    {
        m_repositionThreadStop.store(true);
        if (m_repositionThread->joinable())
            m_repositionThread->join();
        delete m_repositionThread;
    }
}

bool AutopilotPixhawkImpl::setInterval(int sensors, int stat, int rc, int raw, int pos, int extra1, int extra2, int extra3, int adbs, int params)
{
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SYSTEM_TIME, extra1));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_ATTITUDE, extra1));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_AHRS2, extra1));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_PID_TUNING, extra1));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_TIMESYNC, extra2));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_EXTENDED_SYS_STATE, extra2));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_VFR_HUD, extra2));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_AHRS, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_HWSTATUS, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_RANGEFINDER, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_DISTANCE_SENSOR, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_TERRAIN_REQUEST, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_BATTERY2, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_MOUNT_STATUS, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_OPTICAL_FLOW, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_GIMBAL_REPORT, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_MAG_CAL_REPORT, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_EKF_STATUS_REPORT, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_VIBRATION, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_RPM, extra3));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_HIGHRES_IMU, extra3));
    // m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_PARAM_VALUE, params));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_ALTITUDE, pos));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_GLOBAL_POSITION_INT, pos));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_LOCAL_POSITION_NED, pos));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_POSITION_TARGET_GLOBAL_INT, pos));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_RAW_IMU, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SCALED_IMU, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SCALED_IMU2, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SCALED_IMU3, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SCALED_PRESSURE, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SCALED_PRESSURE2, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SCALED_PRESSURE3, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_ACTUATOR_CONTROL_TARGET, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_ODOMETRY, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_ESTIMATOR_STATUS, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_ATTITUDE_QUATERNION, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_ATTITUDE_TARGET, raw));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SERVO_OUTPUT_RAW, rc));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_RC_CHANNELS_SCALED, sensors));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_BATTERY_STATUS, sensors));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_SYS_STATUS, stat));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_POWER_STATUS, stat));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_GPS_RAW_INT, stat));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_GPS_RTK, stat));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_GPS2_RAW, stat));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_GPS2_RTK, stat));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT, stat));
    m_msgInterval.push_back(std::make_pair(MAVLINK_MSG_ID_FENCE_STATUS, stat));

    MavlinkHelper::MavlinkMessageTypeStack *stack =
    new MavlinkHelper::MavlinkMessageTypeStack(10, 100, uav::UavSendMessage::Priority::HIGHT);

    for (const std::pair<int, int> &p : m_msgInterval)
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
                                           p.first,
                                           (p.second < 0) ? -1 : (int)(1000000.f / (float)p.second),
                                           0,
                                           0,
                                           0,
                                           0,
                                           0);
        stack->push(std::move(message));
    }
    m_send(stack);

    return true;
}

bool AutopilotPixhawkImpl::requestARM(bool autoChangeMode, bool force, bool defaultModeAuto)
{
    // BOOST_LOG_TRIVIAL(info) << "DO ARM";
    // if (m_baseMode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED)
    {
        bool readyToStart = false;
        union px4::px4_custom_mode px4_mode;
        px4_mode.data = m_customMode;
        if ((defaultModeAuto && px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_AUTO) ||
            (!defaultModeAuto && (px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_POSCTL || px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_ALTCTL)))
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
                px4_mode.sub_mode  = px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
            }
            else
            {
                px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_ALTCTL; // px4::PX4_CUSTOM_MAIN_MODE_POSCTL ;
                px4_mode.sub_mode  = 0;
            }
            m_target_main_mode      = px4_mode.main_mode;
            m_target_sub_mode       = px4_mode.sub_mode;
            target_force_arm        = force;
            m_waitPrepareToARM      = true;
            m_waitPrepareToARMTimer = std::chrono::system_clock::now();
            sendMode(m_baseMode, px4_mode.data);
            return true;
        }
    }
    return false;
}

bool AutopilotPixhawkImpl::requestDisARM(bool force)
{
    BOOST_LOG_TRIVIAL(info) << "DO DISARM";
    disarm(force);
    return true;
}

bool AutopilotPixhawkImpl::requestTakeOff(const geo::Coords3D &target)
{
    /* Takeoff from ground / hand. Vehicles that support multiple takeoff modes (e.g. VTOL quadplane) should take off
     * using the currently configured mode. |Minimum pitch (if airspeed sensor present), desired pitch without sensor |
     * Empty | Empty | Yaw angle (if magnetometer present), ignored without magnetometer. NaN to use the current system
     * yaw heading mode (e.g. yaw towards next waypoint, yaw to home, etc.). | Latitude | Longitude | Altitude|  */

    BOOST_LOG_TRIVIAL(info) << "DO TAKEOFF " << target.lat() << " " << target.lon() << " " << target.alt();
    mavlink_message_t message;
    union px4::px4_custom_mode px4_mode;
    px4_mode.data = m_customMode;
    if (px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_AUTO)
        mavlink_msg_command_long_pack_chan(m_gcs, 0, m_chanel, &message, m_id, 0, MAV_CMD_MISSION_START, 1, 0, 0, 0, 0, 0, 0, 0);
    else if (px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_POSCTL || px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_ALTCTL)
        mavlink_msg_command_long_pack_chan(m_gcs,
                                           0,
                                           m_chanel,
                                           &message,
                                           m_id,
                                           0,
                                           MAV_CMD_NAV_TAKEOFF,
                                           1,
                                           0,
                                           0,
                                           0,
                                           NAN,
                                           static_cast<float>(target.lat()),
                                           static_cast<float>(target.lon()),
                                           static_cast<float>(target.alt()));
    else
        return false;
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::HIGHT));
    return true;
}

bool AutopilotPixhawkImpl::requestLand()
{
    union px4::px4_custom_mode px4_mode;
    BOOST_LOG_TRIVIAL(info) << "    chnage mode to LAND";
    px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    px4_mode.sub_mode  = px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
    sendMode(m_baseMode, px4_mode.data);
    return true;
}

uav::UAVControlState AutopilotPixhawkImpl::getState(bool &done) const
{
    done = true;
    if (!m_isFlight)
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

bool AutopilotPixhawkImpl::repositionOnboard(const geo::Coords3D &pos, const geo::Coords3D &base)
{
    if (!pos.valid())
        return false;

    std::lock_guard grd(m_repositionLock);

    m_lastRepositionPos = pos;
    m_lastBasePos       = base;
    BOOST_LOG_TRIVIAL(info) << "DO REPOSITION" << pos.lat() << ";" << pos.lon() << ";" << pos.alt() << " YAW " << m_lastYaw;

    union px4::px4_custom_mode px4_mode;
    px4_mode.data = m_customMode;
    if (px4_mode.main_mode != px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD)
    {
        // goto OFFBOARD
        // m_waitForRepositionOFFBOARD = true;
        px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
        px4_mode.sub_mode  = 0;
        sendMode(m_baseMode, px4_mode.data);
        //  return true;
    }

    return true;
}

bool AutopilotPixhawkImpl::repositionOffboard(const geo::Coords3D &pos, const geo::Coords3D &base)
{
    return repositionOnboard(pos, base);
}

bool AutopilotPixhawkImpl::repositionAzimuth(float az)
{
    m_lastYaw = az;
    return repositionOffboard(m_lastRepositionPos, m_lastRepositionPos);
}

void AutopilotPixhawkImpl::setMode(uint8_t base, uint32_t custom)
{
    printMode(m_customMode);

    BOOST_LOG_TRIVIAL(info) << "DO MODE " << (int)base << " " << custom;
    IAutopilot::setMode(m_baseMode, custom);
    if (m_waitPrepareToARM)
    {
        // 2 seconds to start
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::_V2::system_clock::now() - m_waitPrepareToARMTimer);
        if (ms.count() > 2000)
        {
            m_waitPrepareToARM = false;
        }
        union px4::px4_custom_mode px4_mode;
        px4_mode.data = custom;
        if (px4_mode.main_mode != px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD)
        { // rm
            m_repositionThreadWorks.store(false);
            m_remove(MAVLINK_MSG_ID_SET_POSITION_TARGET_GLOBAL_INT);
        }
        if (m_target_main_mode == px4_mode.main_mode && m_target_sub_mode == px4_mode.sub_mode)
        {
            m_waitPrepareToARM = false;
            requestARM(false, target_force_arm);
        }
        else if (px4_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD && m_waitForRepositionOFFBOARD)
        {
            m_repositionThreadWorks.store(true);
            repositionOffboard(m_lastRepositionPos, m_lastBasePos);
        }
    }
}
/*
 * auto_mode_flags  = mavlink.MAV_MODE_FLAG_AUTO_ENABLED \
 *                 | mavlink.MAV_MODE_FLAG_STABILIZE_ENABLED \
 *                 | mavlink.MAV_MODE_FLAG_GUIDED_ENABLED
 *
 *px4_map = { "MANUAL":        (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | mavlink.MAV_MODE_FLAG_STABILIZE_ENABLED |
 *mavlink.MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,   PX4_CUSTOM_MAIN_MODE_MANUAL,      0 ),
 *
 *            "STABILIZED":    (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | mavlink.MAV_MODE_FLAG_STABILIZE_ENABLED |
 *mavlink.MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,   PX4_CUSTOM_MAIN_MODE_STABILIZED,  0 ),
 *
 *            "ACRO":          (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | mavlink.MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,
 *PX4_CUSTOM_MAIN_MODE_ACRO,        0                                       ),
 *
 *            "RATTITUDE":     (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | mavlink.MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,
 *PX4_CUSTOM_MAIN_MODE_RATTITUDE,   0                                       ),
 *
 *            "ALTCTL":        (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | mavlink.MAV_MODE_FLAG_STABILIZE_ENABLED |
 *mavlink.MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,   PX4_CUSTOM_MAIN_MODE_ALTCTL,      0 ),
 *
 *            "POSCTL":        (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | mavlink.MAV_MODE_FLAG_STABILIZE_ENABLED |
 *mavlink.MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,   PX4_CUSTOM_MAIN_MODE_POSCTL,      0 ),
 *
 *            "LOITER":        (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | auto_mode_flags, PX4_CUSTOM_MAIN_MODE_AUTO,
 *PX4_CUSTOM_SUB_MODE_AUTO_LOITER         ),
 *
 *            "MISSION":       (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | auto_mode_flags, PX4_CUSTOM_MAIN_MODE_AUTO,
 *PX4_CUSTOM_SUB_MODE_AUTO_MISSION        ),
 *
 *            "RTL":           (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | auto_mode_flags, PX4_CUSTOM_MAIN_MODE_AUTO,
 *PX4_CUSTOM_SUB_MODE_AUTO_RTL            ),
 *
 *            "FOLLOWME":      (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | auto_mode_flags, PX4_CUSTOM_MAIN_MODE_AUTO,
 *PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET  ),
 *
 *            "OFFBOARD":      (mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | auto_mode_flags,
 *PX4_CUSTOM_MAIN_MODE_OFFBOARD,    0                                       )}
 */

void AutopilotPixhawkImpl::sendMode(uint8_t base, uint32_t custom)
{
    IAutopilot::sendMode(base | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | MAV_MODE_FLAG_GUIDED_ENABLED |
                         MAV_MODE_FLAG_STABILIZE_ENABLED | MAV_MODE_FLAG_AUTO_ENABLED,
                         custom);
}

void AutopilotPixhawkImpl::printMode(uint32_t custom)
{
    union px4::px4_custom_mode px4_mode;
    px4_mode.data = custom;
    switch (px4_mode.main_mode)
    {
        case px4::PX4_CUSTOM_MAIN_MODE_ACRO:
            BOOST_LOG_TRIVIAL(info) << "Mode now: ACRO";
            break;
        case px4::PX4_CUSTOM_MAIN_MODE_ALTCTL:
            BOOST_LOG_TRIVIAL(info) << "Mode now: Altitude control";
            break;
        case px4::PX4_CUSTOM_MAIN_MODE_AUTO:
            switch (px4_mode.sub_mode)
            {
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(follow target)";
                    break;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(land)";
                    break;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(loiter)";
                    break;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(mission)";
                    break;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_READY:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(ready)";
                    break;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_RTGS:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(rtgs)";
                    break;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(RTL)";
                    break;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(Takeoff)";
                    break;
                default:
                    BOOST_LOG_TRIVIAL(info) << "Mode now: AUTO(undefiened)";
                    break;
            }
            break;
        case px4::PX4_CUSTOM_MAIN_MODE_MANUAL:
            BOOST_LOG_TRIVIAL(info) << "Mode now: Manual";
            break;
        case px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD:
            BOOST_LOG_TRIVIAL(info) << "Mode now: offboard";
            break;
        case px4::PX4_CUSTOM_MAIN_MODE_POSCTL:
            BOOST_LOG_TRIVIAL(info) << "Mode now: pos control";
            break;
        case px4::PX4_CUSTOM_MAIN_MODE_RATTITUDE:
            BOOST_LOG_TRIVIAL(info) << "Mode now: ratititude";
            break;
        case px4::PX4_CUSTOM_MAIN_MODE_STABILIZED:
            BOOST_LOG_TRIVIAL(info) << "Mode now: stabilized";
            break;
        default:
            BOOST_LOG_TRIVIAL(info) << "Mode now: undefined";
            break;
    }
}

void AutopilotPixhawkImpl::doRepositionTick()
{
    while (!m_repositionThreadStop.load())
    {
        if (m_repositionThreadWorks.load())
        {
            double lat   = NAN;
            double lon   = NAN;
            double alt   = NAN;
            double yaw   = NAN;
            bool breakit = false;
            {
                std::lock_guard grd(m_repositionLock);
                if (!m_lastRepositionPos.valid())
                    breakit = true;
                else
                {
                    lat = m_lastRepositionPos.lat();
                    lon = m_lastRepositionPos.lon();
                    alt = m_lastRepositionPos.alt();
                }
#ifndef USE_GLOBAL_POSITION
                if (!base.valid())
                    breakit = true;
#endif
                yaw = m_lastYaw;
            }
            if (!breakit)
            {
                auto mask = POSITION_TARGET_TYPEMASK_VX_IGNORE | POSITION_TARGET_TYPEMASK_VY_IGNORE |
                POSITION_TARGET_TYPEMASK_VZ_IGNORE | POSITION_TARGET_TYPEMASK_AX_IGNORE |
                POSITION_TARGET_TYPEMASK_AY_IGNORE | POSITION_TARGET_TYPEMASK_AZ_IGNORE |
                POSITION_TARGET_TYPEMASK_YAW_RATE_IGNORE | POSITION_TARGET_TYPEMASK_FORCE_SET;
                if (std::isnan(yaw))
                    mask |= POSITION_TARGET_TYPEMASK_YAW_IGNORE;

                uint32_t compensate = 0;
                {
                    std::lock_guard grd(m_bootTimeLock);
                    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now =
                    std::chrono::system_clock::now();
                    compensate = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_bootTimeReceived).count();
                }

                mavlink_message_t message;
#ifdef USE_GLOBAL_POSITION
                // Global (WGS84) coordinate frame + MSL altitude. First value / x: latitude, second value / y: longitude, third value / z: positive altitude over mean sea level
                mavlink_msg_set_position_target_global_int_pack_chan(m_gcs,
                                                                     0,
                                                                     m_chanel,
                                                                     &message,
                                                                     m_bootTimeMS + compensate,
                                                                     m_id,
                                                                     0,
                                                                     MAV_FRAME_GLOBAL,
                                                                     mask,
                                                                     (int32_t)(lat * 1.E7),
                                                                     (int32_t)(lon * 1.E7),
                                                                     alt,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     (std::isnan(yaw)) ? 0 : (yaw / 180. * M_PI),
                                                                     0);

#else

                double lat_rad = pos.lat() * M_DEG_TO_RAD;
                double lon_rad = pos.lon() * M_DEG_TO_RAD;

                double ref_lon_rad = base.lon() * M_DEG_TO_RAD;
                double ref_lat_rad = base.lat() * M_DEG_TO_RAD;

                double sin_lat   = sin(lat_rad);
                double cos_lat   = cos(lat_rad);
                double cos_d_lon = cos(lon_rad - ref_lon_rad);

                double ref_sin_lat = sin(ref_lat_rad);
                double ref_cos_lat = cos(ref_lat_rad);

                double c = acos(ref_sin_lat * sin_lat + ref_cos_lat * cos_lat * cos_d_lon);
                double k = (fabs(c) < epsilon) ? 1.0 : (c / sin(c));

                double x = k * (ref_cos_lat * sin_lat - ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH;
                double y = k * cos_lat * sin(lon_rad - ref_lon_rad) * CONSTANTS_RADIUS_OF_EARTH;

                double z = -(pos.alt() - base.alt());
                mavlink_msg_set_position_target_local_ned_pack_chan(m_gcs,
                                                                    0,
                                                                    m_chanel,
                                                                    &message,
                                                                    m_bootTimeMS + compensate,
                                                                    m_id,
                                                                    0,
                                                                    MAV_FRAME_LOCAL_NED,
                                                                    mask,
                                                                    x,
                                                                    y,
                                                                    z,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    (std::isnan(m_lastYaw)) ? 0 : (m_lastYaw * M_DEG_TO_RAD),
                                                                    0);
#endif
                m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 1, 0, uav::UavSendMessage::Priority::HIGHT)); // every second
            }
        }
        usleep(100000);
    }
}
