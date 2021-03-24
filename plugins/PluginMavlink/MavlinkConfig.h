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
#ifndef MAVLINKCONFIG_H
#define MAVLINKCONFIG_H
#include <config/RunConfiguration.h>

class MavlinkConfig : public ConfigInterface
{
#if defined(MavlinkConfig_def)
#error Redefenition MavlinkConfig_def
#else
#define MavlinkConfig_def
#endif
public:
    MavlinkConfig(ConfigInterface *parent);
    virtual ~MavlinkConfig() = default;
    /*!
     * \brief name ApplicationConfiguration
     * \return ApplicationConfiguration
     */
    virtual std::string name() const override final;
    /*!
     * \brief toNode to yaml node
     * \param file root
     * \return
     */
    virtual YAML::Node toNode(const YAML::Node &file) const override final;
    /*!
     * \brief fromNode from yaml node
     * \param node node
     */
    virtual void fromNode(const YAML::Node &node) override final;

    int rateADSB() const;

    int rateExtra1() const;

    int rateExtra2() const;

    int rateExtra3() const;

    int rateParams() const;

    int ratePos() const;

    int rateRaw() const;

    int rateRC() const;

    int rateSensors() const;

    int rateStat() const;

private:
    void setRateADSB(int rateADSB);
    void setRateExtra1(int rateExtra1);
    void setRateExtra2(int rateExtra2);
    void setRateExtra3(int rateExtra3);
    void setRateParams(int rateParams);
    void setRatePos(int ratePos);
    void setRateRaw(int rateRaw);
    void setRateRC(int rateRC);
    void setRateSensors(int rateSensors);
    void setRateStat(int rateStat);

private:
    int m_rateADSB;
    int m_rateExtra1;
    int m_rateExtra2;
    int m_rateExtra3;
    int m_rateParams;
    int m_ratePos;
    int m_rateRaw;
    int m_rateRC;
    int m_rateSensors;
    int m_rateStat;
};

#endif // MAVLINKCONFIG_H
