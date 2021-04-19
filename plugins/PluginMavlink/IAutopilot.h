#ifndef IAUTOPILOT_H
#define IAUTOPILOT_H
#include "MavlinkHelper.h"
#include <functional>
#include <geo/Coords3D.h>
#include <thread>

class IAutopilot
{
public:
    IAutopilot(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    virtual ~IAutopilot();

    MavlinkHelper::ProcessingMode processingMode() const;
    void setProcessingMode(const MavlinkHelper::ProcessingMode &processingMode);

    bool ready() const;
    void setReady(bool ready);

    virtual bool
    setInterval(int sensors, int stat, int rc, int raw, int pos, int extra1, int extra2, int extra3, int adbs, int params) = 0;

    virtual bool requestARM(bool autoChangeMode, bool force, bool defaultModeAuto = false) = 0;
    virtual bool requestDisARM(bool force)                                                 = 0;
    virtual bool requestTakeOff(const geo::Coords3D &target)                               = 0;

    virtual bool repositionOnboard(const geo::Coords3D &pos, const geo::Coords3D &base)  = 0;
    virtual bool repositionOffboard(const geo::Coords3D &pos, const geo::Coords3D &base) = 0;
    virtual uav::UAVControlState getState(bool &done) const                              = 0;
    virtual bool repositionAzimuth(float az)                                             = 0;

    void sendRTCM(const tools::CharMap &cm);
    void sendGetHomePoint();

    virtual bool requestLand() = 0;

    virtual void setMode(uint8_t base, uint32_t custom);
    //
    void setSend(const std::function<void(MavlinkHelper::MavlinkMessageTypeI *)> &send);

    //
    void setIsFlight(bool value);

    void setBootTimeMS(const uint32_t &bootTimeMS);

    void setRemove(const std::function<void(int removeAsId)> &remove);

    void sendSpeed(float ms);

    void ping();

    void doWriteMissionPath(const uav::MissionPath &path, int from = 0);

protected:
    virtual void sendMode(uint8_t base, uint32_t custom);
    void arm(bool force = false);
    void disarm(bool force = false);

    std::function<void(MavlinkHelper::MavlinkMessageTypeI *)> m_send;
    std::function<void(int removeAsId)> m_remove;
    const int m_chanel;
    const int m_gcs;
    const int m_id;

    uint8_t m_baseMode;
    uint32_t m_customMode;

    uint32_t m_bootTimeMS;
    const size_t MAX_RTCM_L;
    uint32_t m_rtcmSeq;

    bool m_isFlight = false;

    std::mutex m_bootTimeLock;
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> m_bootTimeReceived;

    std::thread *m_pingThread = nullptr;
    std::atomic_bool m_pingThreadStop;

    // mission write stack

    uav::MissionPath m_pathCopy;
    bool m_writeProcess           = false;
    bool m_nextMissionIndex       = 0;
    bool m_nextMissionIndexInPath = 0;

private:
    MavlinkHelper::ProcessingMode m_processingMode;
    bool m_ready;
};

#endif // IAUTOPILOT_H
