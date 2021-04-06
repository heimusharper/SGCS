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

    class Message : public UavTask
    {
    public:
        Message(int target)
        : UavTask(target), satelitesGPS(0), satelitesGLONASS(0), hdop(255), vdop(255), fix(FixType::NOGPS)
        {
        }
        tools::optional<uint8_t> satelitesGPS;
        tools::optional<uint8_t> satelitesGLONASS;
        tools::optional<uint8_t> hdop;
        tools::optional<uint8_t> vdop;
        tools::optional<FixType> fix;
    };

    class OnChangeGPSCallback
    {
    public:
        virtual ~OnChangeGPSCallback() = default;
        virtual void updateSatelitesCount()
        {
        }
        virtual void updateErros()
        {
        }
        virtual void updateFixType()
        {
        }
        virtual void sendRTCM(const tools::CharMap &rtcm)
        {
        }
    };

    GPS();
    virtual ~GPS();

    void process(GPS::Message *message);

    uint8_t provGPS() const;
    uint8_t provGLONASS() const;

    uint8_t hdop() const;
    uint8_t vdop() const;

    GPS::FixType fixType() const;

    //
    void addCallback(OnChangeGPSCallback *call);
    void removeCallback(OnChangeGPSCallback *call);

    // RTCM messages
    void sendRTCM(const tools::CharMap &rtcm);

protected:
    void setProvGPS(uint8_t provGPS);
    void setProvGLONASS(uint8_t provGLONASS);
    void setHdop(const uint8_t &hdop);
    void setVdop(const uint8_t &vdop);
    void setFixType(const GPS::FixType &fixType);

private:
    uint8_t m_provGPS     = 0;
    uint8_t m_provGLONASS = 0;

    uint8_t m_hdop = 255;
    uint8_t m_vdop = 255;

    FixType m_fixType;

    std::list<OnChangeGPSCallback *> m_GPSCallback;
};
}

#endif // UAVGPS_H
