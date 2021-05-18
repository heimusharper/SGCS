#ifndef AUTOPILOTAPMIMPL_H
#define AUTOPILOTAPMIMPL_H
#include "IAutopilot.h"
#include "ardupilotmega/ardupilotmega.h"

class AutopilotAPMImpl : public IAutopilot
{
public:
    AutopilotAPMImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    // messages
    void setHeartbeat(const mavlink_heartbeat_t &hrt);
    void setStatusText(const std::string &text);
    void setParamValue(const std::string &text, float value);
    bool magCal(bool start);

public:
    virtual bool requestARM(bool autoChangeMode, bool force, bool defaultModeAuto = false) override final;
    virtual bool requestDisARM(bool force) override final;
    virtual bool requestTakeOff(const geo::Coords3D &target) override final;
    virtual bool repositionOnboard(const geo::Coords3D &pos, const geo::Coords3D &base) override final;
    virtual bool repositionOffboard(const geo::Coords3D &pos, const geo::Coords3D &base) override final;
    virtual bool repositionAzimuth(float az) override final;
    virtual bool requestLand();
    virtual bool requestRTL();

protected:
    virtual bool
    setInterval(int sensors, int stat, int rc, int raw, int pos, int extra1, int extra2, int extra3, int adbs, int params) override final;
    void setProcessingMode(const MavlinkHelper::ProcessingMode &processingMode);
    virtual void sendMode(uint8_t base, uint32_t custom) override final;

protected:
    virtual uav::UAVControlState getState(bool &done) const override final;

    uint32_t modeStart() const;
    uint32_t modeAuto() const;
    uint32_t modeGuided() const;

private:
    bool m_waitPrepareToARM;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> m_waitPrepareToARMTimer;
    bool m_targetForceArm;
    uint32_t m_targetMode;
    bool m_isFlight;
    bool m_isArmProcess;
    bool m_isTakeoffProcess;
    bool m_isCheckIsVTOL;
};

#endif // AUTOPILOTAPMIMPL_H
