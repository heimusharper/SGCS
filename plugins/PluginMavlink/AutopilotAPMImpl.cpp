#include "AutopilotAPMImpl.h"

AutopilotAPMImpl::AutopilotAPMImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: IAutopilot(chan, gcsID, id, mode)
{
}

bool AutopilotAPMImpl::setInterval(IAutopilot::MessageType type, int interval_ms)
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
        mavlink_msg_param_set_pack_chan(
        m_gcs, 0, m_chanel, &message, m_id, 0, id.c_str(), (interval_ms < 0) ? 0 : std::floor(1000 / interval_ms), MAV_PARAM_TYPE_UINT8);
        m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::LOW));
    }
    return true;
}
