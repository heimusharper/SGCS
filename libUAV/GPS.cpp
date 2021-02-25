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
GPS::GPS() : UavObject()
{
}

GPS::~GPS()
{
}

void GPS::process(GPS::Message *message)
{
    if (has(HAS::HAS_PROVIDER_GLONASS))
        if (message->satelitesGLONASS.dirty())
            setProvGLONASS(message->satelitesGLONASS.get());
    if (has(HAS::HAS_PROVIDER_GPS))
        if (message->satelitesGPS.dirty())
            setProvGPS(message->satelitesGPS.get());
    if (has(HAS::HAS_HV_DOP))
    {
        if (message->hdop.dirty())
            setHdop(message->hdop.get());
        if (message->vdop.dirty())
            setVdop(message->vdop.get());
    }
}

uint8_t GPS::provGPS() const
{
    return m_provGPS;
}

void GPS::setProvGPS(uint8_t provGPS)
{
    if (m_provGPS == provGPS)
        return;
    m_provGPS = provGPS;
    BOOST_LOG_TRIVIAL(info) << "GPS " << (int)m_provGPS;
}

uint8_t GPS::provGLONASS() const
{
    return m_provGLONASS;
}

void GPS::setProvGLONASS(uint8_t provGLONASS)
{
    if (m_provGLONASS == provGLONASS)
        return;
    m_provGLONASS = provGLONASS;
    BOOST_LOG_TRIVIAL(info) << "GLONASS " << (int)m_provGLONASS;
}

uint8_t GPS::vdop() const
{
    return m_vdop;
}

void GPS::setVdop(const uint8_t &vdop)
{
    if (m_vdop == vdop)
        return;
    m_vdop = vdop;
    BOOST_LOG_TRIVIAL(info) << "VDOP " << (int)m_vdop;
}

uint8_t GPS::hdop() const
{
    return m_hdop;
}

void GPS::setHdop(const uint8_t &hdop)
{
    if (m_hdop == hdop)
        return;
    m_hdop = hdop;
    BOOST_LOG_TRIVIAL(info) << "HDOP " << (int)m_hdop;
}
}
