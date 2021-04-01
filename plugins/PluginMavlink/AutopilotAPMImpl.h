#ifndef AUTOPILOTAPMIMPL_H
#define AUTOPILOTAPMIMPL_H
#include "IAutopilot.h"

class AutopilotAPMImpl : public IAutopilot
{
public:
    AutopilotAPMImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    virtual bool setInterval(MessageType type, int interval_hz) override final;
    virtual bool requestARM(bool autoChangeMode, bool force, bool defaultModeAuto = false) override final;
    virtual bool requestDisARM(bool force) override final;
    virtual bool requestTakeOff(const geo::Coords3D &target) override final;
    virtual uav::UAVControlState getState(bool &done) const override final;

    virtual bool repositionOnboard(const geo::Coords3D &pos) override final;
    virtual bool repositionOffboard(const geo::Coords3D &pos) override final;
    virtual bool repositionAzimuth(float az) override final;

    virtual bool requestLand()
    {
        return false;
    }

private:
    int m_srid = 0;
};

#endif // AUTOPILOTAPMIMPL_H
