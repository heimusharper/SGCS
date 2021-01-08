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
#ifndef WINDOWCONF_H
#define WINDOWCONF_H

#include "RunConfiguration.h"
#include <QObject>
/*!
 * \brief The WindowConf class
 * Some window preferences
 */
class WindowConf : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)

public:
    explicit WindowConf(QObject *parent = nullptr);

    /*!
     * \brief version get version string (1.2.3-abcd1234)
     * \return
     *  read only
     */
    QString version() const;

private:
    QString m_version;

signals:
    void versionChanged(QString version);
};

#endif // WINDOWCONF_H
