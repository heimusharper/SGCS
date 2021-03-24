#ifndef AUTOPILOTAPMIMPL_H
#define AUTOPILOTAPMIMPL_H
#include "IAutopilot.h"

class AutopilotAPMImpl : public IAutopilot
{
public:
    AutopilotAPMImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    virtual bool setInterval(MessageType type, int interval_ms) override final;

private:
    int m_srid = 0;
};

#endif // AUTOPILOTAPMIMPL_H
