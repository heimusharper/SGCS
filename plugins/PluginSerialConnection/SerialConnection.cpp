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
#include "SerialConnection.h"

SerialConnection::SerialConnection()
: sgcs::connection::Connection()
, m_portName(RunConfiguration::instance().get<SerialConfig>()->portName())
, m_baudRate(RunConfiguration::instance().get<SerialConfig>()->baudRate())
{
}

SerialConnection::~SerialConnection()
{
    _serial->deleteLater();
}

void SerialConnection::inittializeObjects()
{
    connect(this, &SerialConnection::connectToPort, this, &SerialConnection::doConnectToPort);
    connect(this, &SerialConnection::disconnectFromPort, this, &SerialConnection::doDisconnectFromPort);

    _serial = new QSerialPort();
    connect(_serial, &QSerialPort::readyRead, this, &SerialConnection::readyRead);
    connect(_serial, &QSerialPort::errorOccurred, this, &SerialConnection::onError);

    if (!m_portName.isEmpty() && m_baudRate >= 9600)
    {
        // ready to connect automaticaly
        doConnectToPort(m_portName, m_baudRate);
    }
}

void SerialConnection::onTransmit(const QByteArray &data)
{
    if (_serial->isOpen())
        _serial->write(data);
    else
    {
        _writeBuffer.append(data);
        if (_writeBuffer.size() > MAX_BUFFER_SIZE)
            _writeBuffer.remove(0, _writeBuffer.size() - (MAX_BUFFER_SIZE + MAX_BUFFER_SIZE * 0.2));
    }
}

void SerialConnection::onError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::SerialPortError::NoError)
        return;
    QString err = _serial->errorString();
    qWarning() << err;
    emit onDisconnected(err);
}

void SerialConnection::doConnectToPort(const QString &portName, int baudRate)
{
    m_portName = portName;
    m_baudRate = baudRate;

    _serial->setPortName(portName);
    _serial->setBaudRate(baudRate);
    _serial->setDataBits(QSerialPort::Data8);
    _serial->setFlowControl(QSerialPort::NoFlowControl);
    _serial->setParity(QSerialPort::NoParity);
    _serial->setStopBits(QSerialPort::OneStop);
    if (_serial->open(QIODevice::ReadWrite))
    {
        qInfo() << "Serial connected " << m_portName << m_baudRate;
        if (!_writeBuffer.isEmpty())
            _serial->write(_writeBuffer);
        emit onConnected(portName, baudRate);
    }
    else
    {
        QString err = _serial->errorString();
        qWarning() << err;
        emit onDisconnected(err);
    }
}

void SerialConnection::doDisconnectFromPort()
{
    _serial->close();
    emit onDisconnected(QString());
}

void SerialConnection::readyRead()
{
    emit onReceive(_serial->readAll());
}
