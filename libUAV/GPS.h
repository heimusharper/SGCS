/*
 * This file is part of the SGCS distribution (https://github.com/heimusharper/SGCS).
 * Copyright (c) 2020 Andrey Stepanov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef UAVGPS_H
#define UAVGPS_H
#include "UavObject.h"
#include <mutex>

namespace uav
{
class GPS : public UavObject<uint16_t>
{
public:
    enum HAS : uint16_t
    {
        HAS_PROVIDER_GPS     = 0b1 << 0,
        HAS_PROVIDER_GLONASS = 0b1 << 1,
        HAS_HV_DOP           = 0b1 << 2
    };

    enum class FixType : uint8_t
    {
        NOGPS,
        FIX3D,
        RTK
    };

    class OnChangeGPSCallback
    {
    public:
        virtual ~OnChangeGPSCallback() = default;
        /*virtual void updateSatelitesCount(uint8_t gps, uint8_t glonass)
        {
        }
        virtual void updateErros()
        {
        }
        virtual void updateFixType()
        {
        }*/
        virtual void sendRTCM(const tools::CharMap &rtcm)
        {
        }
    };

    GPS();
    virtual ~GPS();

    void getProv(uint8_t &gps, uint8_t &glonass);
    void setProv(uint8_t provGPS, uint8_t provGLONASS);

    void dop(float &h, float &v);
    void setDop(float h, float v);

    void fixType(GPS::FixType &type);
    void setFixType(const GPS::FixType &fixType);

    //
    void addCallback(OnChangeGPSCallback *call);
    void removeCallback(OnChangeGPSCallback *call);

    // RTCM messages
    void sendRTCM(const tools::CharMap &rtcm);

private:
    std::mutex m_provLock;
    uint8_t m_provGPS;
    uint8_t m_provGLONASS;

    std::mutex m_dopLock;
    float m_hdop;
    float m_vdop;

    std::mutex m_fixTypeMtx;
    FixType m_fixType;

    std::list<OnChangeGPSCallback *> m_GPSCallback;
};
}

#endif // UAVGPS_H
