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
    virtual bool requestTakeOff() override final;

    virtual bool repositionOnboard(geo::Coords3D &&pos) override final;
    virtual bool repositionOffboard(geo::Coords3D &&pos) override final;

private:
    int m_srid = 0;
};

#endif // AUTOPILOTAPMIMPL_H
