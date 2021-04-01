#include "IAutopilot.h"

IAutopilot::IAutopilot(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: m_chanel(chan)
, m_gcs(gcsID)
, m_id(id)
, m_baseMode(0)
, m_customMode(0)
, m_bootTimeMS(0)
, m_isFlight(false)
, m_bootTimeReceived(std::chrono::_V2::system_clock::now())
, m_processingMode(mode)
, m_ready(false)
{
    m_pingThreadStop.store(false);
}

IAutopilot::~IAutopilot()
{
    m_pingThreadStop.store(true);
    if (m_pingThread)
    {
        if (m_pingThread->joinable())
            m_pingThread->join();
        delete m_pingThread;
    }
}

MavlinkHelper::ProcessingMode IAutopilot::processingMode() const
{
    return m_processingMode;
}

void IAutopilot::setProcessingMode(const MavlinkHelper::ProcessingMode &processingMode)
{
    m_processingMode = processingMode;
}

bool IAutopilot::ready() const
{
    return m_ready;
}

void IAutopilot::setReady(bool ready)
{
    m_ready = ready;
}

void IAutopilot::setMode(uint8_t base, uint32_t custom)
{
    m_baseMode   = base;
    m_customMode = custom;
}

void IAutopilot::setSend(const std::function<void(MavlinkHelper::MavlinkMessageType *)> &send)
{
    m_send = send;
}

void IAutopilot::sendMode(uint8_t base, uint32_t custom)
{
    mavlink_message_t message;
    mavlink_msg_set_mode_pack_chan(m_gcs, 0, m_chanel, &message, m_id, base, custom);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 200, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::arm(bool force)
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(
    m_gcs, 1, m_chanel, &message, m_id, 0, MAV_CMD_COMPONENT_ARM_DISARM, 1, 1, (force) ? 21196 : 0, 0, 0, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 200, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::disarm(bool force)
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(
    m_gcs, 1, m_chanel, &message, m_id, 0, MAV_CMD_COMPONENT_ARM_DISARM, 1, 0, (force) ? 21196 : 0, 0, 0, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 200, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::setRemove(const std::function<void(int removeAsId)> &remove)
{
    m_remove = remove;
}

void IAutopilot::sendSpeed(float ms)
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(m_gcs, 1, m_chanel, &message, m_id, 0, MAV_CMD_DO_CHANGE_SPEED, 1, 1, ms, -1, 0, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 200, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::ping()
{
    if (m_pingThread)
        return;
    m_pingThread = new std::thread([this]() {
        while (!m_pingThreadStop.load())
        {
            mavlink_message_t message;
            mavlink_msg_heartbeat_pack_chan(m_gcs, 1, m_chanel, &message, m_id, 0, 0, 0, 0);
            m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 1, 1000, uav::UavSendMessage::Priority::HIGHT));
            BOOST_LOG_TRIVIAL(info) << "PING..";
            usleep(1000000);
        }
    });
}

void IAutopilot::setBootTimeMS(const uint32_t &bootTimeMS)
{
    m_bootTimeReceived = std::chrono::_V2::system_clock::now();
    m_bootTimeMS       = bootTimeMS;
}

void IAutopilot::setIsFlight(bool value)
{
    if (m_isFlight == value)
        return;
    BOOST_LOG_TRIVIAL(info) << "DETECTED FLIGHT IS " << value;
    m_isFlight = value;
}
