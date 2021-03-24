#include "IAutopilot.h"

IAutopilot::IAutopilot(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: m_chanel(chan), m_gcs(gcsID), m_id(id), m_processingMode(mode)
{
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

void IAutopilot::setSend(const std::function<void(MavlinkHelper::MavlinkMessageType *)> &send)
{
    m_send = send;
}
