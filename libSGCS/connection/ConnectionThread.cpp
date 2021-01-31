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
#include "ConnectionThread.h"
namespace sgcs
{
namespace connection
{
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

void ConnectionThread::create(Connection *connection, const QList<plugin::ProtocolPlugin *> &protos)
{
    QList<uav::UavProtocol *> protosImpl;
    for (auto x : protos)
    {
        auto inst = x->instance();
        if (inst)
            protosImpl.append(inst);
    }
    _router = new ConnectionRouter(connection, protosImpl);
    _thread = new QThread();
    connect(_thread, &QThread::started, _router, &ConnectionRouter::run);
    connect(_thread, &QThread::finished, _router, &QObject::deleteLater);
    _router->moveToThread(_thread);
    for (auto x : protosImpl)
        x->moveToThread(_thread);
    connection->moveToThread(_thread);
    _thread->start();
}

ConnectionRouter *ConnectionThread::router() const
{
}
}
}
