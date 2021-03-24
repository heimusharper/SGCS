#ifndef IAUTOPILOT_H
#define IAUTOPILOT_H
#include "MavlinkHelper.h"
#include <functional>

class IAutopilot
{
public:
    enum class MessageType
    {
        SENSORS,
        STAT,
        RC,
        RAW,
        POS,
        EXTRA1,
        EXTRA2,
        EXTRA3,
        ADSB,
        PARAMS
    };

    IAutopilot(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);

    MavlinkHelper::ProcessingMode processingMode() const;
    void setProcessingMode(const MavlinkHelper::ProcessingMode &processingMode);

    bool ready() const;
    void setReady(bool ready);

    virtual bool setInterval(MessageType type, int interval_hz) = 0;

    void setSend(const std::function<void(MavlinkHelper::MavlinkMessageType *)> &send);

protected:
    std::function<void(MavlinkHelper::MavlinkMessageType *)> m_send;
    const int m_chanel;
    const int m_gcs;
    const int m_id;

private:
    MavlinkHelper::ProcessingMode m_processingMode;
    bool m_ready;
};

#endif // IAUTOPILOT_H
