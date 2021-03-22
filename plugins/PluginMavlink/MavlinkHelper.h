#ifndef MAVLINKHELPER_H
#define MAVLINKHELPER_H
#include "all/mavlink.h"
#include "mavlink_types.h"
#include <UAV.h>

struct MavlinkHelper
{
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
    enum class Autopilot
    {
        APM,
        PIXHAWK,
        INVALID
    };

    struct Processing
    {
        ProcessingMode pm;
        Autopilot ap;
    };

    static Autopilot mavlinkAutopilot2SGCS(MAV_AUTOPILOT ap)
    {
        switch (ap)
        {
            case MAV_AUTOPILOT_ARDUPILOTMEGA:
                return Autopilot::APM;
                break;
            case MAV_AUTOPILOT_INVALID:
                return Autopilot::INVALID;
            case MAV_AUTOPILOT_GENERIC:
            case MAV_AUTOPILOT_RESERVED:
            case MAV_AUTOPILOT_SLUGS:
            case MAV_AUTOPILOT_OPENPILOT:
            case MAV_AUTOPILOT_GENERIC_WAYPOINTS_ONLY:
            case MAV_AUTOPILOT_GENERIC_WAYPOINTS_AND_SIMPLE_NAVIGATION_ONLY:
            case MAV_AUTOPILOT_GENERIC_MISSION_FULL:
            case MAV_AUTOPILOT_PPZ:
            case MAV_AUTOPILOT_UDB:
            case MAV_AUTOPILOT_FP:
            case MAV_AUTOPILOT_PX4:
            case MAV_AUTOPILOT_SMACCMPILOT:
            case MAV_AUTOPILOT_AUTOQUAD:
            case MAV_AUTOPILOT_ARMAZILA:
            case MAV_AUTOPILOT_AEROB:
            case MAV_AUTOPILOT_ASLUAV:
            case MAV_AUTOPILOT_SMARTAP:
            case MAV_AUTOPILOT_AIRRAILS:
            default:
                return Autopilot::PIXHAWK;
        }
        return Autopilot::INVALID;
    }

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
