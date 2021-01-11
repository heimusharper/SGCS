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
#include "SerialConnectionSelectorListModel.h"

SerialConnectionSelectorListModel::SerialConnectionSelectorListModel(QObject *parent) : QAbstractListModel(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SerialConnectionSelectorListModel::updatePorts);
}

SerialConnectionSelectorListModel::~SerialConnectionSelectorListModel()
{
}

QList<QSerialPortInfo> SerialConnectionSelectorListModel::info() const
{
    return m_info;
}

int SerialConnectionSelectorListModel::current() const
{
    return m_current;
}

bool SerialConnectionSelectorListModel::check() const
{
    return m_check;
}

void SerialConnectionSelectorListModel::setCurrent(int current)
{
    if (m_current == current)
        return;

    m_current = current;
    emit currentChanged(m_current);
}

void SerialConnectionSelectorListModel::setCheck(bool check)
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

void SerialConnectionSelectorListModel::updatePorts()
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

QHash<int, QByteArray> SerialConnectionSelectorListModel::roleNames() const
{
    return {{NameRole, "name"}, {IsBusyRole, "busy"}};
}

int SerialConnectionSelectorListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return info().size();
}

bool SerialConnectionSelectorListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!hasIndex(index.row(), index.column(), index.parent()) || !value.isValid())
        return false;

    // QSerialPortInfo item = info().at(index.row());
    emit dataChanged(index, index, {role});
    return true;
}

QVariant SerialConnectionSelectorListModel::data(const QModelIndex &index, int role) const
{
    if (!hasIndex(index.row(), index.column(), index.parent()))
        return {};
    QSerialPortInfo item = info().at(index.row());
    if (role == NameRole)
    {
        return (item.description().isEmpty()) ? item.portName() : item.portName() + " (" + item.description() + ")";
    }
    if (role == IsBusyRole)
    {
        if ((item.hasProductIdentifier() && item.productIdentifier() == 0) ||
            (item.hasVendorIdentifier() && item.productIdentifier() == 0))
            return false; // TODO: USR-VCOM create 0:0 devices and have lag when get "isBusy"
        return item.isBusy();
    }
    return {};
}
