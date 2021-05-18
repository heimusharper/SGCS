#ifndef IAUTOPILOT_H
#define IAUTOPILOT_H
#include "MavlinkConfig.h"
#include "MavlinkHelper.h"
#include <UAV.h>
#include <functional>
#include <geo/Coords3D.h>
#include <thread>

class IAutopilot
{
public:
    IAutopilot(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode);
    virtual ~IAutopilot();
    /*!
     * \brief getUav UAV instance
     * \return
     */
    uav::UAV *getUav() const;
    void setUav(uav::UAV *uav);
    /*!
     * \brief processingMode Device type
     * \return
     */
    MavlinkHelper::ProcessingMode processingMode() const;
    /*!
     * \brief sendRTCM send GPS RTCM messages
     * \param cm
     */
    void sendRTCM(const tools::CharMap &cm);
    /*!
     * \brief sendGetHomePoint update home point
     */
    void sendGetHomePoint();
    /*!
     * \brief magCal start/stop magnetmeter calibration
     * \param start
     */
    void magCal(bool start);
    /*!
     * \brief setSend message send function handler
     * \param send
     */
    void setSend(const std::function<void(MavlinkHelper::MavlinkMessageTypeI *)> &send);
    /*!
     * \brief setRemove message remove handler
     * \param remove
     */
    void setRemove(const std::function<void(int removeAsId)> &remove);
    /*!
     * \brief doWriteMissionPath write mission path
     * \param path
     * \param from
     */
    void doWriteMissionPath(const uav::MissionPath &path, int from = 0);
    /*!
     * \brief sendSpeed new speed
     * \param ms
     */
    void sendSpeed(float ms);

public:
    // telem
    void setAttitudes(const mavlink_attitude_t &att);
    void setHeartbeat(const mavlink_heartbeat_t &hrt);
    void setGPSRawInt(const mavlink_gps_raw_int_t &gps);
    void setGlobalPositionInt(const mavlink_global_position_int_t &gps);
    void setSysStatus(const mavlink_sys_status_t &stat);
    void setHomePosition(const mavlink_home_position_t &pos);
    void setVFRHud(const mavlink_vfr_hud_t &hud);
    void setAck(const mavlink_command_ack_t &ack);
    void setSystemTime(const mavlink_system_time_t &time);
    void setStatusText(const std::string &text);
    void setParamValue(const std::string &text, float value);
    // requests
    /*!
     * \brief requestARM ARM
     * \param autoChangeMode change mode if wrong
     * \param force
     * \param defaultModeAuto ARM in auto mode
     * \return
     */
    virtual bool requestARM(bool autoChangeMode, bool force, bool defaultModeAuto = false) = 0;
    /*!
     * \brief requestDisARM DISARM
     * \param force
     * \return
     */
    virtual bool requestDisARM(bool force) = 0;
    /*!
     * \brief requestTakeOff TAKEOFF
     * \param target
     * \return
     */
    virtual bool requestTakeOff(const geo::Coords3D &target) = 0;
    /*!
     * \brief repositionOnboard ONBOARD reposition
     * \param pos new position
     * \param base start position
     * \return
     */
    virtual bool repositionOnboard(const geo::Coords3D &pos, const geo::Coords3D &base) = 0;
    /*!
     * \brief repositionOffboard OFFBOARD reposition
     * \param pos new position
     * \param base start position
     * \return
     */
    virtual bool repositionOffboard(const geo::Coords3D &pos, const geo::Coords3D &base) = 0;
    /*!
     * \brief repositionAzimuth set new course
     * \param az course
     * \return
     */
    virtual bool repositionAzimuth(float az) = 0;
    /*!
     * \brief requestLand set land mode
     * \return
     */
    virtual bool requestLand() = 0;
    /*!
     * \brief requestRTL set RTL mode
     * \return
     */
    virtual bool requestRTL() = 0;

    //

private:
    MavlinkHelper::ProcessingMode m_processingMode;

protected:
    virtual bool
    setInterval(int sensors, int stat, int rc, int raw, int pos, int extra1, int extra2, int extra3, int adbs, int params) = 0;
    /*!
     * \brief setProcessingMode set device type
     * \param processingMode device type
     */
    void setProcessingMode(const MavlinkHelper::ProcessingMode &processingMode);
    /*!
     * \brief sendMode send new control modes
     * \param base base mode
     * \param custom custom mode
     */
    virtual void sendMode(uint8_t base, uint32_t custom);
    /*!
     * \brief ping start ping thread
     */
    void ping();
    /*!
     * \brief arm ARM
     * \param force
     */
    void arm(bool force = false);
    /*!
     * \brief disarm DISARM
     * \param force
     */
    void disarm(bool force = false);
    /*!
     * \brief runHearthbeat start hearthbeat
     */
    void hearthbeat();

protected:
    virtual uav::UAVControlState getState(bool &done) const = 0;

protected:
    std::function<void(MavlinkHelper::MavlinkMessageTypeI *)> m_send;
    std::function<void(int removeAsId)> m_remove;

protected:
    /*!
     * \brief m_initialized
     * initialize IAutopilot class
     */
    bool m_initialized = false;
    /*!
     * \brief m_chanel mavlink chanel
     */
    const int M_CHANEL;
    /*!
     * \brief M_GCS GCS ID
     */
    const int M_GCS;
    /*!
     * \brief M_ID TARGET ID
     */
    const int M_ID;
    /*!
     * \brief m_isCheckType
     */
    bool m_isCheckType = false;
    /*!
     * \brief m_baseMode base mode
     */
    uint8_t m_baseMode;
    /*!
     * \brief m_customMode custom mode
     */
    uint32_t m_customMode;
    /*!
     * \brief m_bootTimeMS boot time
     */
    uint32_t m_bootTimeMS;
    // RTCM
    /*!
     * \brief MAX_RTCM_L max RTCM message length
     */
    const size_t MAX_RTCM_L;
    /*!
     * \brief m_rtcmSeq RTCM message sequence
     */
    uint32_t m_rtcmSeq;
    /*!
     * \brief m_isFlight is flight
     */
    bool m_isFlight = false;
    /*!
     * \brief m_uav UAV
     */
    uav::UAV *m_uav = nullptr;
    /*!
     * \brief m_bootTimeLock lock boot time
     */
    std::mutex m_bootTimeLock;
    /*!
     * \brief m_bootTimeReceived boot time updated time
     */
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> m_bootTimeReceived;

private:
    /*!
     * \brief m_ready READY
     */
    bool m_ready;
    std::thread *m_pingThread = nullptr;
    std::atomic_bool m_pingThreadStop;

    /*!
     * \brief m_waitForHomePoint wait home point update
     */
    bool m_waitForHomePoint = true;
    //
    uav::MissionPath m_pathCopy;
    bool m_writeProcess;
    bool m_nextMissionIndex;
    bool m_nextMissionIndexInPath;
};

#endif // IAUTOPILOT_H
