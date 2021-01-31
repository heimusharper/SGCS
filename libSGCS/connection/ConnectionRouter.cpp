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

#include "ConnectionRouter.h"

namespace sgcs
{
namespace connection
{
ConnectionRouter::ConnectionRouter(Connection *connection, const QList<plugin::ProtocolPlugin *> &protos, QObject *parent)
: m_connection(connection), m_protos(protos)
{
}

ConnectionRouter::~ConnectionRouter()
{
}

void ConnectionRouter::run()
{
    m_buffer.resize(1024, 0x00);
    m_connection->inittializeObjects();
    connect(m_connection, &Connection::onReceive, this, &ConnectionRouter::onReceive);

    qDebug() << "RUN ON THREAD ROUTER" << QThread::currentThreadId();
    m_watcher = new QTimer();
    connect(m_watcher, &QTimer::timeout, this, &ConnectionRouter::watch);
    m_watcher->start(1000);
    qDebug() << "HAVE PROTOS!" << m_protos.size();
}

void ConnectionRouter::watch()
{
}

void ConnectionRouter::onReceive(const QByteArray &data)
{
    for (int i = 0; i < data.size(); i++)
        m_buffer.push_back(data.at(i));
}
}
}
