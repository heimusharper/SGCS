#ifndef AUTOPILOTPIXHAWKIMPL_H
#define AUTOPILOTPIXHAWKIMPL_H
#include "IAutopilot.h"

class AutopilotPixhawkImpl : public IAutopilot
{
public:
    AutopilotPixhawkImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    virtual bool setInterval(MessageType type, int interval_hz) override final;
};

#endif // AUTOPILOTPIXHAWKIMPL_H
