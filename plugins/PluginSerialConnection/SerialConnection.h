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
#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H
#include "SerialConfig.h"
#include <QDebug>
#include <QObject>
#include <QSerialPort>
#include <connection/Connection.h>

class SerialConnection : public sgcs::connection::Connection
{
    Q_OBJECT
public:
    explicit SerialConnection();
    virtual ~SerialConnection();
signals:

    void connectToPort(const QString &portName, int baudRate);
    void disconnectFromPort();

protected slots:
    virtual void inittializeObjects() override final;
    virtual void onTransmit(const QByteArray &data) override final;

private slots:

    void onError(QSerialPort::SerialPortError error);
    void doConnectToPort(const QString &portName, int baudRate);
    void doDisconnectFromPort();
    void readyRead();

signals:

    void onConnected(const QString &portName, int baudRate);
    void onDisconnected(const QString &error);

private:
    QString m_portName;
    int m_baudRate            = 0;
    const int MAX_BUFFER_SIZE = 1024;
    QByteArray _writeBuffer;
    QSerialPort *_serial = nullptr;
};

#endif // SERIALCONNECTION_H
