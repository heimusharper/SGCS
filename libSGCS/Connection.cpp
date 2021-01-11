#include "Connection.h"

Connection::Connection() : QObject()
{
}

Connection::~Connection()
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

ConnectionThread::~ConnectionThread()
{
    if (_thread)
    {
        _thread->quit();
        _thread->deleteLater();
    }
}
