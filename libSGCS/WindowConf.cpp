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
#include "WindowConf.h"

WindowConf::WindowConf(QObject *parent) : QObject(parent)
{
    int major    = RunConfiguration::instance().get<ApplicationConfiguration>()->versionMajor();
    int minor    = RunConfiguration::instance().get<ApplicationConfiguration>()->versionMinor();
    int path     = RunConfiguration::instance().get<ApplicationConfiguration>()->versionPath();
    QString hash = RunConfiguration::instance().get<ApplicationConfiguration>()->versionHash();
    m_version    = QString("%1.%2.%3-%4").arg(major).arg(minor).arg(path).arg(hash);
}

QString WindowConf::version() const
{
    return m_version;
}
