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
: QObject()
, sgcs::connection::Connection()
, m_portName(QString::fromStdString(RunConfiguration::instance().get<SerialConfig>()->portName()))
, m_baudRate(RunConfiguration::instance().get<SerialConfig>()->baudRate())
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

SerialConnection::~SerialConnection()
{
    _serial->deleteLater();
}

void SerialConnection::onTransmit(const boost::container::vector<uint8_t> &data)
{
    BOOST_LOG_TRIVIAL(debug) << "WRITE SIZE" << data.size();
    if (_serial->isOpen())
        _serial->write((char *)data.data(), data.size());
    else
    {
        for (int i = 0; i < data.size(); i++)
            _writeBuffer.push(data[i]);
        if (_writeBuffer.size() > MAX_BUFFER_SIZE)
            while (_writeBuffer.size() > MAX_BUFFER_SIZE - MAX_BUFFER_SIZE / 4)
                _writeBuffer.pop();
    }
}

boost::container::vector<uint8_t> SerialConnection::collectBytesAndClear()
{
    boost::container::vector<uint8_t> bytes;
    setHasBytes(false);
    while (!_readBuffer.empty())
    {
        bytes.push_back(_readBuffer.front());
        _readBuffer.pop();
    }
    return bytes;
}

void SerialConnection::onError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::SerialPortError::NoError)
        return;
    QString err = _serial->errorString();
    BOOST_LOG_TRIVIAL(warning) << err.toStdString();
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
        BOOST_LOG_TRIVIAL(info) << "Serial connected " << m_portName.toStdString() << m_baudRate;
        if (!_writeBuffer.empty())
        {
            QByteArray data;
            while (!_writeBuffer.empty())
            {
                data.append(_writeBuffer.front());
                _writeBuffer.pop();
            }
            _serial->write(data);
        }
        emit onConnected(portName, baudRate);
    }
    else
    {
        QString err = _serial->errorString();
        BOOST_LOG_TRIVIAL(warning) << err.toStdString() << portName.toStdString() << baudRate;
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
    auto data = _serial->readAll();
    for (int i = 0; i < data.size(); i++)
        _readBuffer.push(data.at(i));

    if (_readBuffer.size() > MAX_BUFFER_SIZE)
        while (_readBuffer.size() > MAX_BUFFER_SIZE - MAX_BUFFER_SIZE / 4)
            _readBuffer.pop();

    setHasBytes(!_readBuffer.empty());
}
