#include "SerialConnection.h"

SerialConnection::SerialConnection() : Connection()
{
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
