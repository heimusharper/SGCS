#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include "Connection.h"
#include <QDebug>
#include <QObject>
#include <QSerialPort>

class SerialConnection : public Connection
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

    void doConnectToPort(const QString &portName, int baudRate);
    void doDisconnectFromPort();
    void readyRead();

signals:

    void onConnected(const QString &portName, int baudRate);
    void onDisconnected();

private:
    QSerialPort *_serial = nullptr;
};

#endif // SERIALCONNECTION_H
