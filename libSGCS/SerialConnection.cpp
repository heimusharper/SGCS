#include "SerialConnection.h"

SerialConnection::SerialConnection(QObject *parent) : Connection(parent)
{
}

void SerialConnection::onTransmit(const QByteArray &data)
{
}
