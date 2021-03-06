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
#ifndef SERIALCONFIG_H
#define SERIALCONFIG_H
#include <config/RunConfiguration.h>

class SerialConfig : public ConfigInterface
{
#if defined(SerialConfig_def)
#error Redefenition SerialConfig_def
#else
#define SerialConfig_def
#endif
public:
    SerialConfig(ConfigInterface *parent);
    virtual ~SerialConfig() = default;
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

    std::string portName() const;
    void setPortName(const std::string &portName);

    uint32_t baudRate() const;
    void setBaudRate(const uint32_t &baudRate);

    bool autoName() const;
    void setAutoName(bool autoName);

private:
    std::string m_portName;
    uint32_t m_baudRate;
    bool m_autoName;
};

#endif // SERIALCONFIG_H
