#include "UavObject.h"

UavObject::UavObject(QObject *parent) : QObject(parent)
{
}

bool UavObject::connected() const
{
    return m_connected;
}

int UavObject::id() const
{
    return m_id;
}

void UavObject::setConnected(bool connected)
{
    if (m_connected == connected)
        return;

    m_connected = connected;
    emit connectedChanged(m_connected);
}

void UavObject::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged(m_id);
}
