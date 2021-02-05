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
ConnectionRouter::ConnectionRouter(Connection *connection, const QList<uav::UavProtocol *> &protos, QObject *parent)
: m_connection(connection), m_protos(protos)
{
}

ConnectionRouter::~ConnectionRouter()
{
}

void ConnectionRouter::run()
{
    m_buffer.set_capacity(1024);
    m_connection->inittializeObjects();
    connect(m_connection, &Connection::onReceive, this, &ConnectionRouter::onReceive);
    for (uav::UavProtocol *p : m_protos)
    {
        connect(p, &uav::UavProtocol::onReadyData, [this, p]() {
            if (!m_protocol)
            {
                m_protocol = p;
                BOOST_LOG_TRIVIAL(info) << "Ready " << m_protocol->name().toStdString() << " protocol";
                while (!m_protos.empty())
                {
                    auto px = m_protos.takeFirst();
                    if (px != m_protocol)
                    {
                        px->deleteLater();
                    }
                }
                disconnect(m_connection, &Connection::onReceive, this, &ConnectionRouter::onReceive);
                connect(m_connection, &Connection::onReceive, m_protocol, &uav::UavProtocol::onReceived);
            }
        });
    }

    BOOST_LOG_TRIVIAL(debug) << "RUN ON THREAD ROUTER" << QThread::currentThreadId();
    m_watcher = new QTimer();
    connect(m_watcher, &QTimer::timeout, this, &ConnectionRouter::watch);
    m_watcher->start(1000);
    watch();
    BOOST_LOG_TRIVIAL(debug) << "HAVE PROTOS!" << m_protos.size();
}

void ConnectionRouter::watch()
{
    if (m_protocol)
        return;
    for (auto p : m_protos)
    {
        if (!p->hello().isEmpty())
            m_connection->onTransmit(p->hello()); // hello ping!
    }
    if (m_buffer.empty())
        return;

    QByteArray data;
    {
        QMutexLocker lock(&_mutex);
        while (!m_buffer.empty())
        {
            data.append(m_buffer.front());
            m_buffer.pop_front();
        }
    }
    for (auto p : m_protos)
    {
        p->onReceived(data);
    }
}

void ConnectionRouter::onReceive(const QByteArray &data)
{
    if (m_protocol)
        return;
    QMutexLocker lock(&_mutex);
    for (int i = 0; i < data.size(); i++)
        m_buffer.push_back(data.at(i));
}
}
}
