#ifndef AUTOPILOTPIXHAWKIMPL_H
#define AUTOPILOTPIXHAWKIMPL_H
#include "IAutopilot.h"

class AutopilotPixhawkImpl : public IAutopilot
{
public:
    AutopilotPixhawkImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    virtual bool setInterval(MessageType type, int interval_hz) override final;
    virtual bool requestARM(bool autoChangeMode, bool force, bool defaultModeAuto = false) override final;
    virtual bool requestDisARM(bool force) override final;
    virtual bool requestTakeOff() override final;

    virtual bool repositionOnboard(geo::Coords3D &&pos) override final;
    virtual bool repositionOffboard(geo::Coords3D &&pos) override final;

    virtual void setMode(uint8_t base, uint32_t custom) override final;

private:
    int m_waitPrepareToARM = 0;
    uint8_t target_main_mode;
    uint8_t target_sub_mode;
    bool target_force_arm = false;
};

namespace px4
{
enum PX4_CUSTOM_MAIN_MODE
{
    PX4_CUSTOM_MAIN_MODE_MANUAL = 1,
    PX4_CUSTOM_MAIN_MODE_ALTCTL,
    PX4_CUSTOM_MAIN_MODE_POSCTL,
    PX4_CUSTOM_MAIN_MODE_AUTO,
    PX4_CUSTOM_MAIN_MODE_ACRO,
    PX4_CUSTOM_MAIN_MODE_OFFBOARD,
    PX4_CUSTOM_MAIN_MODE_STABILIZED,
    PX4_CUSTOM_MAIN_MODE_RATTITUDE
};

enum PX4_CUSTOM_SUB_MODE_AUTO
{
    PX4_CUSTOM_SUB_MODE_AUTO_READY = 1,
    PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF,
    PX4_CUSTOM_SUB_MODE_AUTO_LOITER,
    PX4_CUSTOM_SUB_MODE_AUTO_MISSION,
    PX4_CUSTOM_SUB_MODE_AUTO_RTL,
    PX4_CUSTOM_SUB_MODE_AUTO_LAND,
    PX4_CUSTOM_SUB_MODE_AUTO_RTGS,
    PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET
};

union px4_custom_mode
{
    struct
    {
        uint16_t reserved;
        uint8_t main_mode;
        uint8_t sub_mode;
    };
    uint32_t data;
    float data_float;
};

}

#endif // AUTOPILOTPIXHAWKIMPL_H
