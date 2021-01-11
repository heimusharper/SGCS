#include "Connection.h"

Connection::Connection() : QObject()
{
}

void Connection::run()
{
    inittializeObjects();
}

void Connection::onReceive(const QByteArray &data)
{
}

ConnectionThread::ConnectionThread(QObject *parent) : QObject(parent)
{
}
