#ifndef AUTOPILOTPIXHAWKIMPL_H
#define AUTOPILOTPIXHAWKIMPL_H
#include "IAutopilot.h"
#include <UAV.h>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <cmath>
#include <thread>

#define USE_GLOBAL_POSITION
//#undef USE_GLOBAL_POSITION

#define M_DEG_TO_RAD M_PI / 180.
#define CONSTANTS_ONE_G 9.80665f                   /* m/s^2		*/
#define CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C 1.225f /* kg/m^3		*/
#define CONSTANTS_AIR_GAS_CONST 287.1f             /* J/(kg * K)		*/
#define CONSTANTS_ABSOLUTE_NULL_CELSIUS -273.15f   /* °C			*/
#define CONSTANTS_RADIUS_OF_EARTH 6371000          /* meters (m)		*/
static const float epsilon = std::numeric_limits<double>::epsilon();

class AutopilotPixhawkImpl : public IAutopilot
{
public:
    AutopilotPixhawkImpl(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    ~AutopilotPixhawkImpl();
    // messages
    void setHeartbeat(const mavlink_heartbeat_t &hrt);
    void setStatusText(const std::string &text);

public:
    virtual bool requestARM(bool autoChangeMode, bool force, bool defaultModeAuto = false) override final;
    virtual bool requestDisARM(bool force) override final;
    virtual bool requestTakeOff(const geo::Coords3D &target) override final;
    virtual bool repositionOnboard(const geo::Coords3D &pos, const geo::Coords3D &base) override final;
    virtual bool repositionOffboard(const geo::Coords3D &pos, const geo::Coords3D &base) override final;
    virtual bool repositionAzimuth(float az) override final;
    virtual bool requestLand() override final;
    virtual bool requestRTL() override final;

protected:
    virtual bool
    setInterval(int sensors, int stat, int rc, int raw, int pos, int extra1, int extra2, int extra3, int adbs, int params) override final;
    virtual void sendMode(uint8_t base, uint32_t custom) override final;
    /*!
     * \brief doRepositionTick do reposition
     */
    void doRepositionTick();

protected:
    virtual uav::UAVControlState getState(bool &done) const override final;
    void printMode(uint32_t custom);

private:
    bool m_waitPrepareToARM;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> m_waitPrepareToARMTimer;
    bool m_waitForRepositionOFFBOARD;
    uint8_t m_target_main_mode;
    uint8_t m_target_sub_mode;
    bool m_targetForceArm;
    geo::Coords3D m_lastRepositionPos;
    geo::Coords3D m_lastBasePos;
    double m_lastYaw;
    std::list<std::pair<int, int>> m_msgInterval;
    std::mutex m_repositionLock;
    std::atomic_bool m_repositionThreadWorks;
    std::atomic_bool m_repositionThreadStop;
    std::thread *m_repositionThread;
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
