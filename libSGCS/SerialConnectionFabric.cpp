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
#include "SerialConnectionFabric.h"

SerialConnectionFabric *SerialConnectionFabric::m_instance = nullptr;

SerialConnectionFabric::SerialConnectionFabric(QObject *parent) : QAbstractListModel(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SerialConnectionFabric::updatePorts);
}

QList<QSerialPortInfo> SerialConnectionFabric::info() const
{
    return m_info;
}

int SerialConnectionFabric::current() const
{
    return m_current;
}

bool SerialConnectionFabric::check() const
{
    return m_check;
}

void SerialConnectionFabric::setCurrent(int current)
{
    if (m_current == current)
        return;

    m_current = current;
    emit currentChanged(m_current);
}

void SerialConnectionFabric::setCheck(bool check)
{
    if (m_check == check)
        return;

    m_check = check;
    if (m_check)
    {
        updatePorts();
        m_timer->start(2000);
    }
    else
        m_timer->stop();
    emit checkChanged(m_check);
}

void SerialConnectionFabric::updatePorts()
{
    QSerialPortInfo lastCurrent; // remember last current
    {
        // clear all
        beginRemoveRows(QModelIndex(), 0, m_info.size() - 1);
        if (current() != -1 && current() < m_info.size())
            lastCurrent = m_info.at(current());
        m_info.clear();
        endRemoveRows();
    }
    {
        // new serials
        auto info = QSerialPortInfo::availablePorts();
        beginInsertRows(QModelIndex(), 0, info.size() - 1);
        m_info = info;
        endInsertRows();
        // find last current
        int newIndex = -1;
        if (!lastCurrent.portName().isEmpty())
        {
            for (int i = 0; i < m_info.size(); i++)
            {
                if (m_info.at(i).portName() == lastCurrent.portName())
                {
                    newIndex = i;
                    break;
                }
            }
        }
        setCurrent(newIndex);
    }
}

void SerialConnectionFabric::create()
{
    qmlRegisterSingletonType<SerialConnectionFabric>("SGCS", 1, 0, "SerialConnectionFabric", SerialConnectionFabric::singletonProvider);
}

SerialConnectionFabric *SerialConnectionFabric::instance()
{
    if (m_instance == nullptr)
        m_instance = new SerialConnectionFabric();
    return m_instance;
}

QHash<int, QByteArray> SerialConnectionFabric::roleNames() const
{
    return {{NameRole, "name"}, {IsBusyRole, "busy"}};
}

int SerialConnectionFabric::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return SerialConnectionFabric::instance()->info().size();
}

bool SerialConnectionFabric::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!hasIndex(index.row(), index.column(), index.parent()) || !value.isValid())
        return false;

    QSerialPortInfo item = SerialConnectionFabric::instance()->info().at(index.row());
    emit dataChanged(index, index, {role});
    return true;
}

QVariant SerialConnectionFabric::data(const QModelIndex &index, int role) const
{
    if (!hasIndex(index.row(), index.column(), index.parent()))
        return {};
    QSerialPortInfo item = SerialConnectionFabric::instance()->info().at(index.row());
    if (role == NameRole)
        return item.portName();
    if (role == IsBusyRole)
        return item.isBusy();
    return {};
}
