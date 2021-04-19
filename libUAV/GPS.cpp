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
#include "GPS.h"

namespace uav
{
GPS::GPS() : UavObject(), m_provGPS(0), m_provGLONASS(0), m_hdop(0), m_vdop(0), m_fixType(FixType::NOGPS)
{
}

GPS::~GPS()
{
}

void GPS::getProv(uint8_t &gps, uint8_t &glonass)
{
    std::lock_guard grd(m_provLock);
    gps     = m_provGPS;
    glonass = m_provGLONASS;
}

void GPS::setProv(uint8_t provGPS, uint8_t provGLONASS)
{
    {
        std::lock_guard grd(m_provLock);
        if (m_provGPS == provGPS && m_provGLONASS == provGLONASS)
            return;
        m_provGPS     = provGPS;
        m_provGLONASS = provGLONASS;
    }
    // for (auto x : m_GPSCallback)
    //    x->updateSatelitesCount(provGPS, provGLONASS);
}

void GPS::fixType(GPS::FixType &type)
{
    std::lock_guard grd(m_fixTypeMtx);
    type = m_fixType;
}

void GPS::addCallback(GPS::OnChangeGPSCallback *call)
{
    m_GPSCallback.push_back(call);
}

void GPS::removeCallback(GPS::OnChangeGPSCallback *call)
{
    m_GPSCallback.remove(call);
}

void GPS::sendRTCM(const tools::CharMap &rtcm)
{
    for (auto x : m_GPSCallback)
        x->sendRTCM(rtcm);
}

void GPS::setFixType(const GPS::FixType &fixType)
{
    {
        std::lock_guard grd(m_fixTypeMtx);
        if (m_fixType == fixType)
            return;
        m_fixType = fixType;
    }
    // for (auto x : m_GPSCallback)
    //    x->updateFixType();
}

void GPS::dop(float &h, float &v)
{
    std::lock_guard grd(m_dopLock);
    h = m_hdop;
    v = m_vdop;
}

void GPS::setDop(float h, float v)
{
    {
        std::lock_guard grd(m_dopLock);
        if (m_hdop == h && m_vdop == v)
            return;
        m_hdop = h;
        m_vdop = v;
    }
    // for (auto x : m_GPSCallback)
    //    x->updateErros();
}

}
