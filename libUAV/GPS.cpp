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

uint8_t GPS::provGPS() const
{
    return m_provGPS.get();
}

void GPS::setProvGPS(uint8_t provGPS)
{
    if (m_provGPS.get() == provGPS)
        return;
    m_provGPS.set(std::move(provGPS));
    for (auto x : m_GPSCallback)
        x->updateSatelitesCount();
}

uint8_t GPS::provGLONASS() const
{
    return m_provGLONASS.get();
}

void GPS::setProvGLONASS(uint8_t provGLONASS)
{
    if (m_provGLONASS.get() == provGLONASS)
        return;
    m_provGLONASS.set(std::move(provGLONASS));
    for (auto x : m_GPSCallback)
        x->updateSatelitesCount();
}

float GPS::vdop() const
{
    return m_vdop.get();
}

void GPS::setVdop(float &&vdop)
{
    if (m_vdop.get() == vdop)
        return;
    m_vdop.set(std::move(vdop));
    for (auto x : m_GPSCallback)
        x->updateErros();
}

GPS::FixType GPS::fixType()
{
    // std::lock_guard grd(m_fixTypeMtx);
    return m_fixType;
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
    std::lock_guard grd(m_fixTypeMtx);
    {
        if (m_fixType == fixType)
            return;
        m_fixType = fixType;
    }
    for (auto x : m_GPSCallback)
        x->updateFixType();
}

float GPS::hdop() const
{
    return m_hdop.get();
}

void GPS::setHdop(float &&hdop)
{
    if (m_hdop.get() == hdop)
        return;
    m_hdop.set(std::move(hdop));
    for (auto x : m_GPSCallback)
        x->updateErros();
}
}
