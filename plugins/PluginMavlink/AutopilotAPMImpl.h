#ifndef AUTOPILOTAPMIMPL_H
#define AUTOPILOTAPMIMPL_H
#include "IAutopilot.h"
#include "ardupilotmega/ardupilotmega.h"

class AutopilotAPMImpl : public IAutopilot
{
public:
    AutopilotAPMImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    virtual bool
    setInterval(int sensors, int stat, int rc, int raw, int pos, int extra1, int extra2, int extra3, int adbs, int params) override final;
    virtual bool requestARM(bool autoChangeMode, bool force, bool defaultModeAuto = false) override final;
    virtual bool requestDisARM(bool force) override final;
    virtual bool requestTakeOff(const geo::Coords3D &target) override final;
    virtual uav::UAVControlState getState(bool &done) const override final;
    virtual bool requestRTL()
    {
        return false;
    }
    virtual bool magCal(bool start)
    {
        return false;
    }

    virtual bool repositionOnboard(const geo::Coords3D &pos, const geo::Coords3D &base) override final;
    virtual bool repositionOffboard(const geo::Coords3D &pos, const geo::Coords3D &base) override final;
    virtual bool repositionAzimuth(float az) override final;

    virtual bool requestLand()
    {
        return false;
    }

    uint32_t modeStart() const;

private:
    int m_srid = 0;
};

#endif // AUTOPILOTAPMIMPL_H
