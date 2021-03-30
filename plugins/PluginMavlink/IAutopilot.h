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
    virtual bool requestTakeOff(int altitude)                                              = 0;

    virtual bool repositionOnboard(const geo::Coords3D &pos)  = 0;
    virtual bool repositionOffboard(const geo::Coords3D &pos) = 0;
    virtual uav::UAVControlState getState(bool &done) const   = 0;
    virtual bool repositionAzimuth(float az)                  = 0;

    virtual void setMode(uint8_t base, uint32_t custom);
    //
    void setSend(const std::function<void(MavlinkHelper::MavlinkMessageType *)> &send);

    //
    void setIsFlight(bool value);

    void setBootTimeMS(const uint32_t &bootTimeMS);

    void setRemove(const std::function<void(int removeAsId)> &remove);

protected:
    virtual void sendMode(uint8_t base, uint32_t custom);
    void arm(bool force = false);
    void disarm(bool force = false);

    std::function<void(MavlinkHelper::MavlinkMessageType *)> m_send;
    std::function<void(int removeAsId)> m_remove;
    const int m_chanel;
    const int m_gcs;
    const int m_id;

    uint8_t m_baseMode;
    uint32_t m_customMode;

    uint32_t m_bootTimeMS;

    bool isFlight = false;

private:
    MavlinkHelper::ProcessingMode m_processingMode;
    bool m_ready;
};

#endif // IAUTOPILOT_H
