#include "AutopilotAPMImpl.h"

AutopilotAPMImpl::AutopilotAPMImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: IAutopilot(chan, gcsID, id, mode)
, m_waitPrepareToARM(false)
, m_waitPrepareToARMTimer(std::chrono::system_clock::now())
, m_targetForceArm(false)
, m_isFlight(false)
, m_isArmProcess(false)
, m_isTakeoffProcess(false)
, m_isCheckIsVTOL(false)
{
}

void AutopilotAPMImpl::setHeartbeat(const mavlink_heartbeat_t &hrt)
{
    IAutopilot::setHeartbeat(hrt);
    if (m_waitPrepareToARM)
    {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::_V2::system_clock::now() - m_waitPrepareToARMTimer);
        if (ms.count() > 2000)
        {
            m_waitPrepareToARM = false;
        }
        if (m_targetMode == m_customMode)
        {
            m_targetMode       = INT32_MAX;
            m_waitPrepareToARM = false;
            requestARM(false, m_targetForceArm);
        }
    }
    if (m_targetMode == modeGuided())
    {
        m_targetMode = INT32_MAX;
        repositionOffboard(geo::Coords3D(), geo::Coords3D());
    }
    if (m_isTakeoffProcess && m_isFlight)
        m_isTakeoffProcess = false;
}

void AutopilotAPMImpl::setStatusText(const std::string &text)
{
    IAutopilot::setStatusText(text);
}

void AutopilotAPMImpl::setParamValue(const std::string &text, float value)
{
    IAutopilot::setParamValue(text, value);
    if (text.find("Q_ENABLE") != std::string::npos)
    {
        if (std::abs(value - 1) < 0.1)
        {
            m_isCheckType = true;
            setProcessingMode(MavlinkHelper::ProcessingMode::UAV_VTOL);
        }
    }
}

bool AutopilotAPMImpl::setInterval(int sensors, int stat, int rc, int raw, int pos, int extra1, int extra2, int extra3, int adbs, int params)
{
    std::list<std::pair<std::string, uint>> m_msgInterval;
    m_msgInterval.push_back(std::make_pair("SR1_ADSB", adbs));
    m_msgInterval.push_back(std::make_pair("SR1_EXTRA1", extra1));
    m_msgInterval.push_back(std::make_pair("SR1_EXTRA2", extra2));
    m_msgInterval.push_back(std::make_pair("SR1_EXTRA3", extra3));
    m_msgInterval.push_back(std::make_pair("SR1_PARAMS", params));
    m_msgInterval.push_back(std::make_pair("SR1_POSITION", pos));
    m_msgInterval.push_back(std::make_pair("SR1_RAW_CTRL", raw));
    m_msgInterval.push_back(std::make_pair("SR1_RAW_SENS", sensors));
    m_msgInterval.push_back(std::make_pair("SR1_RC_CHAN", rc));
    m_msgInterval.push_back(std::make_pair("SR1_EXT_STAT", stat));

    for (const std::pair<std::string, int> &id : m_msgInterval)
    {
        mavlink_message_t message;
        mavlink_msg_param_set_pack_chan(M_GCS, 0, M_CHANEL, &message, M_ID, 0, id.first.c_str(), id.second, MAV_PARAM_TYPE_UINT8);
        m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::LOW));
    }
    return true;
}

void AutopilotAPMImpl::setProcessingMode(const MavlinkHelper::ProcessingMode &processingMode)
{
    AutopilotAPMImpl::setProcessingMode(processingMode);
    if (processingMode == MavlinkHelper::ProcessingMode::UAV_PLANE && !m_isCheckIsVTOL)
    {
        mavlink_message_t message;
        mavlink_msg_param_request_read_pack_chan(M_GCS, 0, M_CHANEL, &message, M_ID, 0, "Q_ENABLE", -1);
        m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::LOW));
    }
}

void AutopilotAPMImpl::sendMode(uint8_t base, uint32_t custom)
{
    IAutopilot::sendMode(base | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | MAV_MODE_FLAG_GUIDED_ENABLED |
                         MAV_MODE_FLAG_STABILIZE_ENABLED | MAV_MODE_FLAG_AUTO_ENABLED,
                         custom);
}

bool AutopilotAPMImpl::requestARM(bool autoChangeMode, bool force, bool defaultModeAuto)
{
    m_isTakeoffProcess = false;
    if (m_customMode == modeStart())
    {
        arm(force);
        return true;
    }
    else if (autoChangeMode)
    {
        // mode change
        if (defaultModeAuto)
            m_targetMode = modeAuto();
        else
            m_targetMode = modeStart();
        m_targetForceArm        = force;
        m_waitPrepareToARM      = true;
        m_waitPrepareToARMTimer = std::chrono::system_clock::now();
        m_isArmProcess          = true;
        sendMode(m_baseMode, m_targetMode);
        return true;
    }
    return false;
}

bool AutopilotAPMImpl::requestDisARM(bool force)
{
    m_waitPrepareToARM = false;
    disarm(force);
    return false;
}

bool AutopilotAPMImpl::requestTakeOff(const geo::Coords3D &target)
{
    BOOST_LOG_TRIVIAL(info) << "DO TAKEOFF " << target.lat() << " " << target.lon() << " " << target.alt();
    mavlink_message_t message;
    if (m_customMode == modeAuto())
        mavlink_msg_command_long_pack_chan(M_GCS, 0, M_CHANEL, &message, M_ID, 0, MAV_CMD_MISSION_START, 1, 0, 0, 0, 0, 0, 0, 0);
    else if (m_customMode == modeStart())
        mavlink_msg_command_long_pack_chan(M_GCS,
                                           0,
                                           M_CHANEL,
                                           &message,
                                           M_ID,
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
    m_isTakeoffProcess = true;
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::HIGHT));
    return true;
}
bool AutopilotAPMImpl::requestLand()
{
    uint32_t mode = 0;
    switch (processingMode())
    {
        case MavlinkHelper::ProcessingMode::UAV_VTOL:
        case MavlinkHelper::ProcessingMode::UAV_PLANE:
            mode = PLANE_MODE_QLAND;
            // move rally point
            break;
        case MavlinkHelper::ProcessingMode::UAV_MC:
            mode = COPTER_MODE_LAND;
            break;
        default:
            break;
    }
    if (mode != 0)
    {
        sendMode(m_baseMode, mode);
        return true;
    }
    return false;
}

bool AutopilotAPMImpl::requestRTL()
{
    uint32_t mode = 0;
    switch (processingMode())
    {
        case MavlinkHelper::ProcessingMode::UAV_VTOL:
        case MavlinkHelper::ProcessingMode::UAV_PLANE:
            mode = PLANE_MODE_RTL;
            // move rally point
            break;
        case MavlinkHelper::ProcessingMode::UAV_MC:
            mode = COPTER_MODE_RTL;
            break;
        default:
            break;
    }
    if (mode != 0)
    {
        sendMode(m_baseMode, mode);
        return true;
    }
    return false;
}

bool AutopilotAPMImpl::magCal(bool start)
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(
    M_GCS, 0, M_CHANEL, &message, M_ID, 0, MAV_CMD_PREFLIGHT_CALIBRATION, 0, 0, (start) ? 1 : 0, 0, 0, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::HIGHT)); // every second
    return true;
}

uav::UAVControlState AutopilotAPMImpl::getState(bool &done) const
{
    done = true;
    if ((m_baseMode & 0b10000000) != 0) // armed
    {
        //
        if (m_isArmProcess)
            return uav::UAVControlState::PREPARED;
        else if (m_isTakeoffProcess)
            return uav::UAVControlState::STARTED;
        else
        {
            switch (processingMode())
            {
                case MavlinkHelper::ProcessingMode::UAV_VTOL:
                case MavlinkHelper::ProcessingMode::UAV_PLANE:
                {
                    switch (m_customMode)
                    {
                        case PLANE_MODE_RTL:
                            return uav::UAVControlState::RTL;
                        case PLANE_MODE_AUTO:
                            return uav::UAVControlState::AUTO;
                        case PLANE_MODE_GUIDED:
                            return uav::UAVControlState::MANUAL_OFFBOARD;
                        case PLANE_MODE_TRAINING:
                        case PLANE_MODE_TAKEOFF:
                        case PLANE_MODE_STABILIZE:
                        case PLANE_MODE_QSTABILIZE:
                        case PLANE_MODE_QRTL:
                        case PLANE_MODE_QLOITER:
                        case PLANE_MODE_QLAND:
                        case PLANE_MODE_QHOVER:
                        case PLANE_MODE_QAUTOTUNE:
                        case PLANE_MODE_QACRO:
                        case PLANE_MODE_MANUAL:
                        case PLANE_MODE_LOITER:
                        case PLANE_MODE_INITIALIZING:
                        case PLANE_MODE_FLY_BY_WIRE_B:
                        case PLANE_MODE_FLY_BY_WIRE_A:
                        case PLANE_MODE_CRUISE:
                        case PLANE_MODE_CIRCLE:
                        case PLANE_MODE_AVOID_ADSB:
                        case PLANE_MODE_ACRO:
                        case PLANE_MODE_AUTOTUNE:
                            return uav::UAVControlState::UNDEFINED;
                    }
                    break;
                }
                case MavlinkHelper::ProcessingMode::UAV_MC:
                {
                    break;
                }
                case MavlinkHelper::ProcessingMode::ANT:
                case MavlinkHelper::ProcessingMode::CAMERA:
                case MavlinkHelper::ProcessingMode::GIMBAL:
                case MavlinkHelper::ProcessingMode::MODEM:
                case MavlinkHelper::ProcessingMode::UNDEFINED:
                default:
                    break;
            }
        }
    }
    done = false;
    return uav::UAVControlState::WAIT;
}

bool AutopilotAPMImpl::repositionOnboard(const geo::Coords3D &pos, const geo::Coords3D &base)
{
    mavlink_message_t message;
    mavlink_msg_mission_item_pack_chan(M_GCS,
                                       0,
                                       M_CHANEL,
                                       &message,
                                       M_ID,
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

bool AutopilotAPMImpl::repositionOffboard(const geo::Coords3D &pos, const geo::Coords3D &base)
{
    return repositionOnboard(pos, base);
}

bool AutopilotAPMImpl::repositionAzimuth(float az)
{
    return false;
}

uint32_t AutopilotAPMImpl::modeStart() const
{
    switch (processingMode())
    {
        case MavlinkHelper::ProcessingMode::UAV_VTOL:
        case MavlinkHelper::ProcessingMode::UAV_PLANE:
            return PLANE_MODE_LOITER;
        case MavlinkHelper::ProcessingMode::UAV_MC:
            return COPTER_MODE_POSHOLD;
        default:
            break;
    }
    return INT32_MAX;
}

uint32_t AutopilotAPMImpl::modeAuto() const
{
    switch (processingMode())
    {
        case MavlinkHelper::ProcessingMode::UAV_VTOL:
        case MavlinkHelper::ProcessingMode::UAV_PLANE:
            return PLANE_MODE_LOITER;
        case MavlinkHelper::ProcessingMode::UAV_MC:
            return COPTER_MODE_POSHOLD;
        default:
            break;
    }
    return INT32_MAX;
}

uint32_t AutopilotAPMImpl::modeGuided() const
{
    switch (processingMode())
    {
        case MavlinkHelper::ProcessingMode::UAV_VTOL:
        case MavlinkHelper::ProcessingMode::UAV_PLANE:
            return PLANE_MODE_GUIDED;
        case MavlinkHelper::ProcessingMode::UAV_MC:
            return COPTER_MODE_GUIDED;
        default:
            break;
    }
    return INT32_MAX;
}
