#include "AutopilotAPMImpl.h"

AutopilotAPMImpl::AutopilotAPMImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: IAutopilot(chan, gcsID, id, mode)
{
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
        mavlink_msg_param_set_pack_chan(m_gcs, 0, m_chanel, &message, m_id, 0, id.first.c_str(), id.second, MAV_PARAM_TYPE_UINT8);
        m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::LOW));
    }
    return true;
}

bool AutopilotAPMImpl::requestARM(bool autoChangeMode, bool force, bool defaultModeAuto)
{
    bool readyToStart = false;
    if (m_customMode == COPTER_MODE_POSHOLD)
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
    return false;
}

bool AutopilotAPMImpl::requestDisARM(bool force)
{
    return false;
}

bool AutopilotAPMImpl::requestTakeOff(const geo::Coords3D &target)
{
    return false;
}

uav::UAVControlState AutopilotAPMImpl::getState(bool &done) const
{
    done = false;
    return uav::UAVControlState::WAIT;
}

bool AutopilotAPMImpl::repositionOnboard(const geo::Coords3D &pos, const geo::Coords3D &base)
{
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
    switch (m_processingMode)
    {
        case MavlinkHelper::ProcessingMode::UAV_VTOL:
        case MavlinkHelper::ProcessingMode::UAV_PLANE:
            return PLANE_MODE_LOITER;
        case MavlinkHelper::ProcessingMode::UAV_MC:
            return COPTER_MODE_POSHOLD;
            break;
        default:
            break;
    }
    return INT32_MAX;
}
