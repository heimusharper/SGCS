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

SerialConnection::SerialConnection() : Connection()
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
}

void SerialConnection::onTransmit(const QByteArray &data)
{
}

void SerialConnection::doConnectToPort(const QString &portName, int baudRate)
{
    _serial->setPortName(portName);
    _serial->setBaudRate(baudRate);
    _serial->setDataBits(QSerialPort::Data8);
    _serial->setFlowControl(QSerialPort::NoFlowControl);
    _serial->setParity(QSerialPort::NoParity);
    if (_serial->open(QIODevice::ReadWrite))
    {
        emit onConnected(portName, baudRate);
    }
    else
    {
        qWarning() << _serial->errorString();
        emit onDisconnected();
    }
}

void SerialConnection::doDisconnectFromPort()
{
    _serial->close();
    emit onDisconnected();
}

void SerialConnection::readyRead()
{
    qDebug() << _serial->readAll();
}
