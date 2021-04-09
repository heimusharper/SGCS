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
#include "MavlinkConfig.h"

MavlinkConfig::MavlinkConfig(ConfigInterface *parent)
: ConfigInterface(parent)
, m_rateADSB(-1)
, m_rateExtra1(1)
, m_rateExtra2(1)
, m_rateExtra3(1)
, m_rateParams(-1)
, m_ratePos(4)
, m_rateRaw(-1)
, m_rateRC(-1)
, m_rateSensors(1)
, m_rateStat(1)
{
}

std::string MavlinkConfig::name() const
{
    return "Mavlink";
}

YAML::Node MavlinkConfig::toNode(const YAML::Node &file) const
{
    YAML::Node node      = file;
    node["rate ADSB"]    = m_rateADSB;
    node["rate Extra1"]  = m_rateExtra1;
    node["rate Extra2"]  = m_rateExtra2;
    node["rate Extra3"]  = m_rateExtra3;
    node["rate params"]  = m_rateParams;
    node["rate pos"]     = m_ratePos;
    node["rate raw"]     = m_rateRaw;
    node["rate RC"]      = m_rateRC;
    node["rate sensors"] = m_rateSensors;
    node["rate stat"]    = m_rateStat;
    return node;
}

void MavlinkConfig::fromNode(const YAML::Node &node)
{
    if (node["rate ADSB"])
        m_rateADSB = node["rate ADSB"].as<int>();
    if (node["rate Extra1"])
        m_rateExtra1 = node["rate Extra1"].as<int>();
    if (node["rate Extra2"])
        m_rateExtra2 = node["rate Extra2"].as<int>();
    if (node["rate Extra3"])
        m_rateExtra3 = node["rate Extra3"].as<int>();
    if (node["rate params"])
        m_rateParams = node["rate params"].as<int>();
    if (node["rate pos"])
        m_ratePos = node["rate pos"].as<int>();
    if (node["rate raw"])
        m_rateRaw = node["rate raw"].as<int>();
    if (node["rate RC"])
        m_rateRC = node["rate RC"].as<int>();
    if (node["rate sensors"])
        m_rateSensors = node["rate sensors"].as<int>();
    if (node["rate stat"])
        m_rateStat = node["rate stat"].as<int>();
}

int MavlinkConfig::rateADSB() const
{
    return m_rateADSB;
}

void MavlinkConfig::setRateADSB(int rateADSB)
{
    m_rateADSB = rateADSB;
}

int MavlinkConfig::rateExtra1() const
{
    return m_rateExtra1;
}

void MavlinkConfig::setRateExtra1(int rateExtra1)
{
    m_rateExtra1 = rateExtra1;
}

int MavlinkConfig::rateExtra2() const
{
    return m_rateExtra2;
}

void MavlinkConfig::setRateExtra2(int rateExtra2)
{
    m_rateExtra2 = rateExtra2;
}

int MavlinkConfig::rateExtra3() const
{
    return m_rateExtra3;
}

void MavlinkConfig::setRateExtra3(int rateExtra3)
{
    m_rateExtra3 = rateExtra3;
}

int MavlinkConfig::rateParams() const
{
    return m_rateParams;
}

void MavlinkConfig::setRateParams(int rateParams)
{
    m_rateParams = rateParams;
}

int MavlinkConfig::ratePos() const
{
    return m_ratePos;
}

void MavlinkConfig::setRatePos(int ratePos)
{
    m_ratePos = ratePos;
}

int MavlinkConfig::rateRaw() const
{
    return m_rateRaw;
}

void MavlinkConfig::setRateRaw(int rateRaw)
{
    m_rateRaw = rateRaw;
}

int MavlinkConfig::rateRC() const
{
    return m_rateRC;
}

void MavlinkConfig::setRateRC(int rateRC)
{
    m_rateRC = rateRC;
}

int MavlinkConfig::rateSensors() const
{
    return m_rateSensors;
}

void MavlinkConfig::setRateSensors(int rateSensors)
{
    m_rateSensors = rateSensors;
}

int MavlinkConfig::rateStat() const
{
    return m_rateStat;
}

void MavlinkConfig::setRateStat(int rateStat)
{
    m_rateStat = rateStat;
}
