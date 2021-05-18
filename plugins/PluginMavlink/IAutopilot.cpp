#include "IAutopilot.h"

IAutopilot::IAutopilot(int chan, int gcsID, int id, MavlinkHelper::ProcessingMode mode)
: m_processingMode(mode)
, m_initialized(false)
, M_CHANEL(chan)
, M_GCS(gcsID)
, M_ID(id)
, m_isCheckType(false)
, m_baseMode(0)
, m_customMode(0)
, m_bootTimeMS(0)
, MAX_RTCM_L(180)
, m_rtcmSeq(0)
, m_isFlight(false)
, m_bootTimeLock()
, m_bootTimeReceived(std::chrono::_V2::system_clock::now())
, m_ready(false)
, m_writeProcess(false)
, m_nextMissionIndex(0)
, m_nextMissionIndexInPath(0)

{
    m_pingThreadStop.store(false);
}

IAutopilot::~IAutopilot()
{
    m_pingThreadStop.store(true);
    if (m_pingThread)
    {
        if (m_pingThread->joinable())
            m_pingThread->join();
        delete m_pingThread;
    }
}

void IAutopilot::setAttitudes(const mavlink_attitude_t &att)
{
    if (m_uav)
        m_uav->ahrs()->set(static_cast<float>(att.roll / M_PI * 180.),
                           static_cast<float>(att.pitch / M_PI * 180.),
                           static_cast<float>(att.yaw / M_PI * 180.));
}

void IAutopilot::setHeartbeat(const mavlink_heartbeat_t &hrt)
{
    m_baseMode   = hrt.base_mode;
    m_customMode = hrt.custom_mode;
    if (!m_isCheckType)
    {
        m_isCheckType = true;
        setProcessingMode(MavlinkHelper::mavlinkUavType2SGCS((MAV_TYPE)hrt.type));
    }

    if (!m_initialized)
    {
        setInterval(RunConfiguration::instance().get<MavlinkConfig>()->rateSensors(),
                    RunConfiguration::instance().get<MavlinkConfig>()->rateStat(),
                    RunConfiguration::instance().get<MavlinkConfig>()->rateRC(),
                    RunConfiguration::instance().get<MavlinkConfig>()->rateRaw(),
                    RunConfiguration::instance().get<MavlinkConfig>()->ratePos(),
                    RunConfiguration::instance().get<MavlinkConfig>()->rateExtra1(),
                    RunConfiguration::instance().get<MavlinkConfig>()->rateExtra2(),
                    RunConfiguration::instance().get<MavlinkConfig>()->rateExtra3(),
                    RunConfiguration::instance().get<MavlinkConfig>()->rateADSB(),
                    RunConfiguration::instance().get<MavlinkConfig>()->rateParams());
        m_initialized = true;
    }
    bool readyState = false;
    auto state      = getState(readyState);
    if (readyState)
        m_uav->state(state);
}

void IAutopilot::setGPSRawInt(const mavlink_gps_raw_int_t &gps)
{
    float hacc = (float)gps.h_acc / 100.;
    float vacc = (float)gps.v_acc / 100.;
    m_uav->gps()->setDop(hacc, vacc);
    m_uav->gps()->setProv(gps.satellites_visible, 0);

    bool checkHome = false;
    switch (gps.fix_type)
    {
        case GPS_FIX_TYPE_RTK_FIXED:
            m_uav->gps()->setFixType(uav::GPS::FixType::RTK);
            checkHome = true;
            break;
        case GPS_FIX_TYPE_RTK_FLOAT:
            m_uav->gps()->setFixType(uav::GPS::FixType::RTK_FLOAT);
            checkHome = true;
            break;
        case GPS_FIX_TYPE_3D_FIX:
        case GPS_FIX_TYPE_DGPS:
            m_uav->gps()->setFixType(uav::GPS::FixType::FIX3D);
            checkHome = true;
            break;
        case GPS_FIX_TYPE_NO_GPS:
        case GPS_FIX_TYPE_NO_FIX:
        case GPS_FIX_TYPE_2D_FIX:
        case GPS_FIX_TYPE_STATIC:
        case GPS_FIX_TYPE_PPP:
        default:
            m_uav->gps()->setFixType(uav::GPS::FixType::NOGPS);
            break;
    }
    if (checkHome && m_waitForHomePoint)
        sendGetHomePoint();
}

void IAutopilot::setGlobalPositionInt(const mavlink_global_position_int_t &gps)
{
    geo::Coords3D coord(((double)gps.lat) / 1.E7, ((double)gps.lon) / 1.E7, ((double)gps.alt) / 1000.);
    m_uav->position()->setGps(std::move(coord));
}

void IAutopilot::setSysStatus(const mavlink_sys_status_t &stat)
{
    m_uav->power()->setVoltage((double)stat.voltage_battery / 1000.);
    {
        MavlinkHelper::MavlinkSensors health;
        health.value = stat.onboard_control_sensors_health;
        MavlinkHelper::MavlinkSensors enabled;
        enabled.value = stat.onboard_control_sensors_enabled;
        MavlinkHelper::MavlinkSensors present;
        present.value = stat.onboard_control_sensors_present;
        uint16_t err  = 0;
        {
            if (!health.mag3d && present.mag3d && enabled.mag3d)
                err |= uav::Status::MAG;
            if ((!health.accel3d && present.accel3d && enabled.accel3d) ||
                (!health.accel23d && present.accel23d && enabled.accel23d))
                err |= uav::Status::ACCEL;
            if (!health.gps && present.gps && enabled.gps)
                err |= uav::Status::GPS;
        }
        m_uav->status()->setFailures(err);
    }
}

void IAutopilot::setHomePosition(const mavlink_home_position_t &pos)
{
    geo::Coords3D coord(((double)pos.latitude) / 1.E7, ((double)pos.longitude) / 1.E7, ((double)pos.altitude) / 1000.);
    m_uav->home()->setPosition(coord);
    m_waitForHomePoint = false;
}

void IAutopilot::setVFRHud(const mavlink_vfr_hud_t &hud)
{
    m_isFlight = hud.throttle > 10;
}

void IAutopilot::setAck(const mavlink_command_ack_t &ack)
{
    switch (ack.result)
    {
        case MAV_RESULT_ACCEPTED:
        {
            BOOST_LOG_TRIVIAL(info) << "ACK MESSAGE " << ack.command << " ACCEPTED " << (int)ack.progress << " "
                                    << (int)ack.result << " " << ack.result_param2;
            switch (ack.command)
            {
                case MAV_CMD_COMPONENT_ARM_DISARM:
                {
                    sendGetHomePoint(); // update home position
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            BOOST_LOG_TRIVIAL(warning) << "ACK MESSAGE " << ack.command << " FAILED " << (int)ack.result
                                       << (int)ack.progress << " " << ack.result_param2;
            break;
    }
}

void IAutopilot::setSystemTime(const mavlink_system_time_t &time)
{
    std::lock_guard grd(m_bootTimeLock);
    m_bootTimeReceived = std::chrono::_V2::system_clock::now();
    m_bootTimeMS       = time.time_boot_ms;
}

void IAutopilot::setStatusText(const std::string &text)
{
}

void IAutopilot::setParamValue(const std::string &text, float value)
{
}

MavlinkHelper::ProcessingMode IAutopilot::processingMode() const
{
    return m_processingMode;
}

void IAutopilot::setProcessingMode(const MavlinkHelper::ProcessingMode &processingMode)
{
    if (m_processingMode == processingMode)
        return;
    m_processingMode = processingMode;
    switch (m_processingMode)
    {
        case MavlinkHelper::ProcessingMode::ANT:
            break;
        case MavlinkHelper::ProcessingMode::CAMERA:
            break;
        case MavlinkHelper::ProcessingMode::GIMBAL:
            break;
        case MavlinkHelper::ProcessingMode::MODEM:
            break;
        case MavlinkHelper::ProcessingMode::UAV_MC:
            m_uav->setType(uav::UAVType::MULTICOPTER);
            break;
        case MavlinkHelper::ProcessingMode::UAV_PLANE:
            m_uav->setType(uav::UAVType::PLANE);
            break;
        case MavlinkHelper::ProcessingMode::UAV_VTOL:
            m_uav->setType(uav::UAVType::VTOL);
            break;
        case MavlinkHelper::ProcessingMode::UNDEFINED:
            break;
        default:
            break;
    }
}

void IAutopilot::sendRTCM(const tools::CharMap &cm)
{
    if (cm.size > MAX_RTCM_L * 4)
        return; // ignore large messages

    auto createRTCM = [this](int fragment, int seq, const tools::CharMap &map, int from, int size) -> mavlink_message_t {
        // BOOST_LOG_TRIVIAL(info)
        // << "RTCM " << fragment << " sq " << seq << " map " << map.size << " [" << from << "; " << size << "]";
        mavlink_message_t message;
        uint8_t flag = 0;
        if (fragment < 0)
            flag = 0; // одн пакет
        else
            flag = 1; // много пакетов
        flag += (uint8_t)((((fragment < 0) ? 0 : fragment) & 0x3) << 1);
        flag += (uint8_t)((seq & 0x1f) << 3);
        mavlink_msg_gps_rtcm_data_pack_chan(MAV_COMP_ID_ALL, 0, M_CHANEL, &message, flag, size, (const uint8_t *)map.data + from);
        return message;
    };
    if (cm.size > MAX_RTCM_L)
    {
        MavlinkHelper::MavlinkMessageTypeStack *stack =
        new MavlinkHelper::MavlinkMessageTypeStack(3, 50, uav::UavSendMessage::Priority::HIGHT);
        // split
        int fragment   = 0;
        size_t pointer = 0;
        while (pointer < cm.size)
        {
            size_t size = cm.size - pointer;
            if (size > MAX_RTCM_L)
                size = MAX_RTCM_L;
            stack->push(createRTCM(fragment, m_rtcmSeq, cm, pointer, size));
            pointer += size; // смещаем укзатель
            fragment++;
        }
        m_send(stack);
    }
    else
    {
        auto message = createRTCM(-1, m_rtcmSeq, cm, 0, cm.size);
        m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 50, uav::UavSendMessage::Priority::HIGHT));
    }
    m_rtcmSeq++;
}

void IAutopilot::sendGetHomePoint()
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(M_GCS, 0, M_CHANEL, &message, M_ID, 0, MAV_CMD_GET_HOME_POSITION, 0, 0, 0, 0, 0, 1, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 200, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::magCal(bool start)
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(
    M_GCS, 0, M_CHANEL, &message, M_ID, 0, MAV_CMD_PREFLIGHT_CALIBRATION, 0, 0, (start) ? 1 : 0, 0, 0, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 3, 200, uav::UavSendMessage::Priority::HIGHT)); // every second
}

void IAutopilot::setSend(const std::function<void(MavlinkHelper::MavlinkMessageTypeI *)> &send)
{
    m_send = send;
    hearthbeat();
    ping();
}

void IAutopilot::sendMode(uint8_t base, uint32_t custom)
{
    mavlink_message_t message;
    mavlink_msg_set_mode_pack_chan(M_GCS, 0, M_CHANEL, &message, M_ID, base, custom);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 200, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::arm(bool force)
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(
    M_GCS, 1, M_CHANEL, &message, M_ID, 0, MAV_CMD_COMPONENT_ARM_DISARM, 1, 1, (force) ? 21196 : 0, 0, 0, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 200, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::disarm(bool force)
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(
    M_GCS, 1, M_CHANEL, &message, M_ID, 0, MAV_CMD_COMPONENT_ARM_DISARM, 1, 0, (force) ? 21196 : 0, 0, 0, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 200, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::hearthbeat()
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack(255, 0, &message, MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), -1, 1000, uav::UavSendMessage::Priority::LOW));
}

uav::UAV *IAutopilot::getUav() const
{
    return m_uav;
}

void IAutopilot::setUav(uav::UAV *uav)
{
    m_uav = uav;
}

void IAutopilot::setRemove(const std::function<void(int removeAsId)> &remove)
{
    m_remove = remove;
}

void IAutopilot::sendSpeed(float ms)
{
    mavlink_message_t message;
    mavlink_msg_command_long_pack_chan(M_GCS, 1, M_CHANEL, &message, M_ID, 0, MAV_CMD_DO_CHANGE_SPEED, 1, 1, ms, -1, 0, 0, 0, 0);
    m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 5, 100, uav::UavSendMessage::Priority::HIGHT));
}

void IAutopilot::ping()
{
    if (m_pingThread)
        return;
    m_pingThread = new std::thread([this]() {
        while (!m_pingThreadStop.load())
        {
            mavlink_message_t message;
            mavlink_msg_heartbeat_pack_chan(M_GCS, 1, M_CHANEL, &message, M_ID, 0, 0, 0, 0);
            m_send(new MavlinkHelper::MavlinkMessageType(std::move(message), 1, 1000, uav::UavSendMessage::Priority::HIGHT));
            // BOOST_LOG_TRIVIAL(info) << "PING..";
            usleep(1000000);
        }
    });
}

void IAutopilot::doWriteMissionPath(const uav::MissionPath &path, int from)
{
    m_pathCopy               = uav::MissionPath(path);
    m_writeProcess           = true;
    m_nextMissionIndex       = from;
    m_nextMissionIndexInPath = 0;
}
