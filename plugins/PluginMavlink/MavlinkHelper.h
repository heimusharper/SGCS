#ifndef MAVLINKHELPER_H
#define MAVLINKHELPER_H
#include "all/mavlink.h"
#include "mavlink_types.h"
#include <CharMap.h>
#include <UAV.h>

struct MavlinkHelper
{
    class MavlinkMessageType : public uav::UavSendMessage
    {
    public:
        MavlinkMessageType(mavlink_message_t &&mavlink,
                           int ticks                         = 1,
                           int interval                      = 0,
                           UavSendMessage::Priority priority = UavSendMessage::Priority::NORMAL)
        : uav::UavSendMessage(ticks, interval, priority), m_mavlink(mavlink)
        {
        }
        virtual ~MavlinkMessageType() = default;

        virtual tools::CharMap pack() const override final
        {
            tools::CharMap cm;
            cm.data         = new char[MAVLINK_MAX_PACKET_LEN];
            cm.size         = MAVLINK_MAX_PACKET_LEN;
            uint16_t lenght = mavlink_msg_to_send_buffer((uint8_t *)cm.data, &m_mavlink);
            if (lenght > 0)
            {
                // BOOST_LOG_TRIVIAL(info) << "PACKING " << m_mavlink.msgid;
                cm.size = lenght;
                return cm;
            }
            return tools::CharMap();
        }
        mavlink_message_t mavlink() const
        {
            return m_mavlink;
        }

    private:
        mavlink_message_t m_mavlink;
    };

    enum class ProcessingMode
    {
        UAV_VTOL,
        UAV_PLANE,
        UAV_MC,
        MODEM,
        ANT,
        GIMBAL,
        CAMERA,
        UNDEFINED
    };

    static ProcessingMode mavlinkUavType2SGCS(MAV_TYPE type)
    {
        switch (type)
        {
            case MAV_TYPE_VTOL_DUOROTOR:
            case MAV_TYPE_VTOL_QUADROTOR:
            case MAV_TYPE_VTOL_RESERVED2:
            case MAV_TYPE_VTOL_RESERVED3:
            case MAV_TYPE_VTOL_RESERVED4:
            case MAV_TYPE_VTOL_TILTROTOR:
                return ProcessingMode::UAV_VTOL;
            case MAV_TYPE_GENERIC:
            case MAV_TYPE_FLAPPING_WING:
            case MAV_TYPE_FIXED_WING:
                return ProcessingMode::UAV_PLANE;
                break;
            case MAV_TYPE_COAXIAL:
            case MAV_TYPE_HELICOPTER:
            case MAV_TYPE_HEXAROTOR:
            case MAV_TYPE_OCTOROTOR:
            case MAV_TYPE_QUADROTOR:
            case MAV_TYPE_TRICOPTER:
                return ProcessingMode::UAV_MC;
                break;
            case MAV_TYPE_ANTENNA_TRACKER:
                return ProcessingMode::ANT;
                break;
            case MAV_TYPE_GIMBAL:
                return ProcessingMode::GIMBAL;
                break;
            case MAV_TYPE_CAMERA:
                return ProcessingMode::CAMERA;
                break;
            case MAV_TYPE_FLARM:
            case MAV_TYPE_GROUND_ROVER:
            case MAV_TYPE_KITE:
            case MAV_TYPE_PARAFOIL:
            case MAV_TYPE_SUBMARINE:
            case MAV_TYPE_SURFACE_BOAT:
            case MAV_TYPE_DECAROTOR:
            case MAV_TYPE_AIRSHIP:
            case MAV_TYPE_FREE_BALLOON:
            case MAV_TYPE_CHARGING_STATION:
            case MAV_TYPE_GCS:
            case MAV_TYPE_ODID:
            case MAV_TYPE_ONBOARD_CONTROLLER:
            case MAV_TYPE_ROCKET:
            case MAV_TYPE_ADSB:
            case MAV_TYPE_SERVO:
            default:
                return ProcessingMode::UNDEFINED;
                break;
        }
        return ProcessingMode::UNDEFINED;
    }
};
#endif // MAVLINKHELPER_H
