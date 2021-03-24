#include "AutopilotAPMImpl.h"

AutopilotAPMImpl::AutopilotAPMImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: IAutopilot(chan, gcsID, id, mode)
{
}

bool AutopilotAPMImpl::setInterval(IAutopilot::MessageType type, int interval_hz)
{
    std::list<std::string> msg_ids;
    switch (type)
    {
        case MessageType::ADSB:
            msg_ids.push_back("SR1_ADSB");
            break;
        case MessageType::EXTRA1:
            msg_ids.push_back("SR1_EXTRA1");
            break;
        case MessageType::EXTRA2:
            msg_ids.push_back("SR1_EXTRA2");
            break;
        case MessageType::EXTRA3:
            msg_ids.push_back("SR1_EXTRA3");
            break;
        case MessageType::PARAMS:
            msg_ids.push_back("SR1_PARAMS");
            break;
        case MessageType::POS:
            msg_ids.push_back("SR1_POSITION");
            break;
        case MessageType::RAW:
            msg_ids.push_back("SR1_RAW_CTRL");
            msg_ids.push_back("SR1_RAW_SENS");
            break;
        case MessageType::RC:
            msg_ids.push_back("SR1_RC_CHAN");
            break;
        case MessageType::SENSORS:
            break;
        case MessageType::STAT:
            msg_ids.push_back("SR1_EXT_STAT");
            break;
        default:
            break;
    }
    if (msg_ids.empty())
        return false;
    for (const std::string &id : msg_ids)
    {
        mavlink_message_t message;
        mavlink_msg_param_set_pack_chan(m_gcs, 0, m_chanel, &message, m_id, 0, id.c_str(), (interval_hz < 0) ? 0 : interval_hz, MAV_PARAM_TYPE_UINT8);
        m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::LOW));
    }
    return true;
}

bool AutopilotAPMImpl::requestARM(bool autoChangeMode, bool force, bool defaultModeAuto)
{
    // TODO: implement me
    return false;
}

bool AutopilotAPMImpl::requestDisARM(bool force)
{
}

bool AutopilotAPMImpl::requestTakeOff(int altitude)
{
}

bool AutopilotAPMImpl::repositionOnboard(geo::Coords3D &&pos)
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

bool AutopilotAPMImpl::repositionOffboard(geo::Coords3D &&pos)
{
    return repositionOnboard(std::move(pos));
}
