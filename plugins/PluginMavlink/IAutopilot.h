#ifndef IAUTOPILOT_H
#define IAUTOPILOT_H
#include "MavlinkHelper.h"
#include <functional>
#include <geo/Coords3D.h>

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

    virtual bool setInterval(MessageType type, int interval_hz)                            = 0;
    virtual bool requestARM(bool autoChangeMode, bool force, bool defaultModeAuto = false) = 0;
    virtual bool requestDisARM(bool force)                                                 = 0;
    virtual bool requestTakeOff()                                                          = 0;

    virtual bool repositionOnboard(geo::Coords3D &&pos)  = 0;
    virtual bool repositionOffboard(geo::Coords3D &&pos) = 0;

    virtual void setMode(uint8_t base, uint32_t custom);
    //
    void setSend(const std::function<void(MavlinkHelper::MavlinkMessageType *)> &send);

    //

protected:
    void sendMode(uint8_t base, uint32_t custom);
    void arm(bool force = false);
    void disarm(bool force = false);

    std::function<void(MavlinkHelper::MavlinkMessageType *)> m_send;
    const int m_chanel;
    const int m_gcs;
    const int m_id;

    uint8_t m_baseMode;
    uint32_t m_customMode;

private:
    MavlinkHelper::ProcessingMode m_processingMode;
    bool m_ready;
};

#endif // IAUTOPILOT_H
