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

SerialConnectionSelectorListModel::SerialConnectionSelectorListModel(QObject *parent)
: QAbstractListModel(parent), m_current(-1), m_check(false), m_baudrate(57600), m_connected(false)
{
    _connection                = new connection::ConnectionThread();
    SerialConnection *instance = _connection->create<SerialConnection>();

    connect(this, &SerialConnectionSelectorListModel::tryConnect, instance, &SerialConnection::connectToPort);
    connect(this, &SerialConnectionSelectorListModel::tryDisconnect, instance, &SerialConnection::disconnectFromPort);
    connect(instance, &SerialConnection::onConnected, this, &SerialConnectionSelectorListModel::onSerialConnectedTo);
    connect(instance, &SerialConnection::onDisconnected, this, &SerialConnectionSelectorListModel::onSerialDisconnected);
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

int SerialConnectionSelectorListModel::baudrate() const
{
    return m_baudrate;
}

bool SerialConnectionSelectorListModel::connected() const
{
    return m_connected;
}

QString SerialConnectionSelectorListModel::serialError() const
{
    return m_serialError;
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
        timer()->start(2000);
    }
    else
        timer()->stop();
    emit checkChanged(m_check);
}

void SerialConnectionSelectorListModel::setBaudrate(int baudrate)
{
    if (m_baudrate == baudrate)
        return;

    m_baudrate = baudrate;
    emit baudrateChanged(m_baudrate);
}

void SerialConnectionSelectorListModel::setConnected(bool connected)
{
    if (m_connected == connected)
        return;

    qDebug() << "SET CONNECTED" << connected << ((m_current >= 0) ? m_info.at(m_current).portName() : "ttyNA") << m_baudrate;
    if (connected)
    {
        if (m_current >= 0 && m_current < m_info.size() && m_baudrate > 0)
        {
            emit tryConnect(m_info.at(m_current).portName(), m_baudrate);
        }
        else
            return; // failed, return
    }
    else
    {
        emit tryDisconnect();
    }
    m_connected = connected;
    emit connectedChanged(m_connected);
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
        if (!m_info.isEmpty())
        {
            if (lastCurrent.portName().isEmpty())
                lastCurrent = m_info.first();
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

void SerialConnectionSelectorListModel::onSerialConnectedTo(const QString &portName, int baudRate)
{
    int current = -1;
    int i       = 0;
    for (auto p : m_info)
    {
        if (p.portName() == portName)
        {
            current = i;
            break;
        }
        i++;
    }
    setCurrent(current);
    setBaudrate(baudRate);
    setConnected(true);
}

void SerialConnectionSelectorListModel::onSerialDisconnected(const QString &error)
{
    setConnected(false);
    m_serialError = error;
    emit serialErrorChanged(m_serialError);
}

QTimer *SerialConnectionSelectorListModel::timer()
{
    if (!m_timer)
    {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &SerialConnectionSelectorListModel::updatePorts);
    }
    return m_timer;
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
