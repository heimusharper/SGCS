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
#ifndef UAVOBJECT_H
#define UAVOBJECT_H

#include <QObject>

namespace uav
{
class UavObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)

public:
    explicit UavObject(QObject *parent = nullptr);

    bool connected() const;

    int id() const;

public slots:
    void setConnected(bool connected);

    void setId(int id);

signals:
    void connectedChanged(bool connected);
    void idChanged(int id);

private:
    bool m_connected = false;
    int m_id         = -1;
};
}
#endif // UAVOBJECT_H
