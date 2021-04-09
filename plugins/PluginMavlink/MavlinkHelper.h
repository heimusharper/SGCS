#ifndef MAVLINKHELPER_H
#define MAVLINKHELPER_H

#if __GNUC__ > 8
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "all/mavlink.h"
#include "mavlink_types.h"
#include <CharMap.h>
#include <UAV.h>

struct MavlinkHelper
{
    class MavlinkMessageTypeI : public uav::UavSendMessage
    {
    public:
        MavlinkMessageTypeI(int ticks = 1, int interval = 0, UavSendMessage::Priority priority = UavSendMessage::Priority::NORMAL)
        : uav::UavSendMessage(ticks, interval, priority)
        {
        }
        virtual ~MavlinkMessageTypeI() = default;

        virtual tools::CharMap pack() const override final
        {
            tools::CharMap cm;
            if (empty())
                return cm;
            cm.data             = new char[MAVLINK_MAX_PACKET_LEN];
            cm.size             = MAVLINK_MAX_PACKET_LEN;
            mavlink_message_t m = mavlink();
            uint16_t lenght     = mavlink_msg_to_send_buffer((uint8_t *)cm.data, &m);
            if (lenght > 0)
            {
                BOOST_LOG_TRIVIAL(info) << "PACKING " << mavlink().msgid;
                cm.size = lenght;
                return cm;
            }
            return tools::CharMap();
        }
        virtual mavlink_message_t mavlink() const = 0;

    protected:
        virtual bool compare(const UavSendMessage *message) const override final
        {
            const MavlinkHelper::MavlinkMessageType *msg = dynamic_cast<const MavlinkHelper::MavlinkMessageType *>(message);
            if (msg)
            {
                mavlink_message_t msrc = mavlink();
                mavlink_message_t msgt = msg->mavlink();
                if (mavlink().msgid == MAVLINK_MSG_ID_HEARTBEAT || msgt.msgid == MAVLINK_MSG_ID_HEARTBEAT ||
                    mavlink().msgid == MAVLINK_MSG_ID_PING || msgt.msgid == MAVLINK_MSG_ID_PING)
                    return false;
                if (mavlink().msgid == msgt.msgid)
                {
                    switch (mavlink().msgid)
                    {
                        case MAVLINK_MSG_ID_GPS_RTCM_DATA:
                        {
                            mavlink_gps_rtcm_data_t rtcm1;
                            mavlink_msg_gps_rtcm_data_decode(&msrc, &rtcm1);
                            mavlink_gps_rtcm_data_t rtcm2;
                            mavlink_msg_gps_rtcm_data_decode(&msgt, &rtcm2);
                            if (rtcm1.flags != rtcm2.flags)
                                return false;
                        }
                        case MAVLINK_MSG_ID_COMMAND_LONG:
                        {
                            mavlink_command_long_t long1;
                            mavlink_msg_command_long_decode(&msrc, &long1);
                            mavlink_command_long_t long2;
                            mavlink_msg_command_long_decode(&msgt, &long2);
                            if (long1.command != long2.command)
                                return false;
                            break;
                        }
                        case MAVLINK_MSG_ID_PARAM_SET:
                        {
                            mavlink_param_set_t rsp1;
                            mavlink_msg_param_set_decode(&msrc, &rsp1);
                            mavlink_param_set_t rsp2;
                            mavlink_msg_param_set_decode(&msgt, &rsp2);
                            std::string first  = std::string(rsp1.param_id, strnlen(rsp1.param_id, 16));
                            std::string second = std::string(rsp2.param_id, strnlen(rsp2.param_id, 16));
                            if (first.compare(second) != 0)
                                return false;
                            break;
                        }
                        case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
                        {
                            mavlink_param_request_read_t rsp1;
                            mavlink_msg_param_request_read_decode(&msrc, &rsp1);
                            mavlink_param_request_read_t rsp2;
                            mavlink_msg_param_request_read_decode(&msgt, &rsp2);

                            std::string first  = std::string(rsp1.param_id, strnlen(rsp1.param_id, 16));
                            std::string second = std::string(rsp2.param_id, strnlen(rsp2.param_id, 16));
                            if (first.compare(second) != 0)
                                return false;
                            break;
                        }
                        case MAVLINK_MSG_ID_DATA32:
                        {
                            mavlink_data32_t data1;
                            mavlink_msg_data32_decode(&msrc, &data1);
                            mavlink_data32_t data2;
                            mavlink_msg_data32_decode(&msgt, &data2);
                            if (data1.type != data2.type)
                                return false;
                            break;
                        }
                        default:
                            break;
                    }
                    return true;
                }
                else
                {
                    switch (mavlink().msgid)
                    {
                        case MAVLINK_MSG_ID_COMMAND_LONG:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_COMMAND_ACK)
                            {
                                mavlink_command_long_t longd;
                                mavlink_msg_command_long_decode(&msrc, &longd);
                                mavlink_command_ack_t ack;
                                mavlink_msg_command_ack_decode(&msgt, &ack);
                                if (ack.command == longd.command)
                                    return true;
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_SET_MODE:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_COMMAND_ACK)
                            {
                                mavlink_command_ack_t ack;
                                mavlink_msg_command_ack_decode(&msgt, &ack);
                                if (ack.command == MAV_CMD_DO_SET_MODE)
                                {
                                    return true;
                                }
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_COMMAND_ACK:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_COMMAND_LONG)
                            {
                                mavlink_command_long_t longd;
                                mavlink_msg_command_long_decode(&msgt, &longd);
                                mavlink_command_ack_t ack;
                                mavlink_msg_command_ack_decode(&msrc, &ack);
                                if (ack.command == longd.command)
                                    return true;
                            }
                            if (msgt.msgid == MAVLINK_MSG_ID_SET_MODE)
                            {
                                mavlink_command_ack_t ack;
                                mavlink_msg_command_ack_decode(&msrc, &ack);
                                if (ack.command == MAV_CMD_DO_SET_MODE)
                                    return true;
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_MISSION_COUNT)
                                return true;
                            break;
                        }
                        case MAVLINK_MSG_ID_MISSION_REQUEST:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_MISSION_ITEM)
                            {
                                mavlink_mission_item_t it;
                                mavlink_msg_mission_item_decode(&msgt, &it);
                                mavlink_mission_request_t itx;
                                mavlink_msg_mission_request_decode(&msrc, &itx);
                                if (it.seq == itx.seq)
                                    return true;
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_MISSION_SET_CURRENT:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_MISSION_CURRENT)
                            {
                                mavlink_mission_current_t itin;
                                mavlink_msg_mission_current_decode(&msgt, &itin);
                                mavlink_mission_set_current_t it;
                                mavlink_msg_mission_set_current_decode(&msrc, &it);
                                if (itin.seq == it.seq)
                                    return true;
                            }
                        }
                        case MAVLINK_MSG_ID_PARAM_SET:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_PARAM_VALUE)
                            {
                                mavlink_param_value_t rsp;
                                mavlink_msg_param_value_decode(&msgt, &rsp);
                                mavlink_param_set_t rq;
                                mavlink_msg_param_set_decode(&msrc, &rq);
                                std::string first  = std::string(rsp.param_id, strnlen(rsp.param_id, 16));
                                std::string second = std::string(rq.param_id, strnlen(rq.param_id, 16));
                                if (first.compare(second) == 0)
                                    return true;
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_PARAM_VALUE)
                            {
                                mavlink_param_value_t rsp;
                                mavlink_msg_param_value_decode(&msgt, &rsp);
                                mavlink_param_request_read_t rq;
                                mavlink_msg_param_request_read_decode(&msrc, &rq);
                                std::string first  = std::string(rsp.param_id, strnlen(rsp.param_id, 16));
                                std::string second = std::string(rq.param_id, strnlen(rq.param_id, 16));
                                if (first.compare(second) == 0)
                                    return true;
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_MISSION_CLEAR_ALL:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_MISSION_ACK)
                            {
                                mavlink_mission_ack_t ac;
                                mavlink_msg_mission_ack_decode(&msgt, &ac);
                                if (ac.type == MAV_MISSION_ACCEPTED)
                                    return true;
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_MISSION_COUNT:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_MISSION_REQUEST)
                            {
                                return true;
                            }
                            if (msgt.msgid == MAVLINK_MSG_ID_MISSION_ACK)
                            {
                                mavlink_mission_ack_t ac;
                                mavlink_msg_mission_ack_decode(&msgt, &ac);
                                if (ac.type == MAV_MISSION_ACCEPTED)
                                    return true;
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_MISSION_WRITE_PARTIAL_LIST:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_MISSION_REQUEST)
                            {
                                return true;
                            }
                            else if (msgt.msgid == MAVLINK_MSG_ID_MISSION_ACK)
                            {
                                mavlink_mission_ack_t ac;
                                mavlink_msg_mission_ack_decode(&msgt, &ac);
                                if (ac.type == MAV_MISSION_ACCEPTED)
                                    return true;
                            }
                            break;
                        }
                        case MAVLINK_MSG_ID_MISSION_ITEM:
                        {
                            if (msgt.msgid == MAVLINK_MSG_ID_MISSION_REQUEST)
                            {
                                mavlink_mission_item_t item;
                                mavlink_msg_mission_item_decode(&msrc, &item);
                                mavlink_mission_request_t rq;
                                mavlink_msg_mission_request_decode(&msgt, &rq);
                                if (item.seq < rq.seq)
                                    return true;
                            }
                            else if (msgt.msgid == MAVLINK_MSG_ID_MISSION_ACK)
                            {
                                mavlink_mission_ack_t ac;
                                mavlink_msg_mission_ack_decode(&msgt, &ac);
                                if (ac.type == MAV_MISSION_ACCEPTED)
                                    return true;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            return false;
        }
    };

    class MavlinkMessageType : public MavlinkMessageTypeI
    {
    public:
        MavlinkMessageType(mavlink_message_t &&mavlink,
                           int ticks                         = 1,
                           int interval                      = 0,
                           UavSendMessage::Priority priority = UavSendMessage::Priority::NORMAL)
        : MavlinkMessageTypeI(ticks, interval, priority), m_mavlink(mavlink)
        {
        }
        virtual bool empty() const override final
        {
            return isEmpty;
        }
        virtual void pop() override final
        {
            isEmpty = true;
            MavlinkMessageTypeI::pop();
        }
        mavlink_message_t mavlink() const
        {
            return m_mavlink;
        }

    private:
        bool isEmpty = false;
        mavlink_message_t m_mavlink;
    };
    class MavlinkMessageTypeStack : public MavlinkMessageTypeI
    {
    public:
        MavlinkMessageTypeStack(int ticks = 1, int interval = 0, UavSendMessage::Priority priority = UavSendMessage::Priority::NORMAL)
        : MavlinkMessageTypeI(ticks, interval, priority)
        {
        }
        virtual bool empty() const override final
        {
            return m_mavlink.empty();
        }
        virtual void pop() override final
        {
            m_mavlink.pop_front();

            BOOST_LOG_TRIVIAL(warning) << "MESSAGES AFTER POP " << m_mavlink.size();
            MavlinkMessageTypeI::pop();
        }
        void push(mavlink_message_t &&m)
        {
            m_mavlink.push_back(m);
        }
        mavlink_message_t mavlink() const
        {
            if (m_mavlink.empty())
                return mavlink_message_t();
            return m_mavlink.front();
        }

    private:
        std::list<mavlink_message_t> m_mavlink;
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
