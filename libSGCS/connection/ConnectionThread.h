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
#ifndef CONNECTIONTHREAD_H
#define CONNECTIONTHREAD_H

#include "ConnectionRouter.h"

namespace connection
{
class ConnectionThread : public QObject
{
    Q_OBJECT
public:
    ConnectionThread(QObject *parent = nullptr);
    ~ConnectionThread();

    template <typename T>
    T *create()
    {
        qDebug() << "RUN ON THREAD ROOT" << QThread::currentThreadId();
        T *connection = new T;
        _router       = new ConnectionRouter(connection);

        _thread = new QThread();
        connect(_thread, &QThread::started, _router, &ConnectionRouter::run);
        connect(_thread, &QThread::finished, _router, &QObject::deleteLater);
        _router->moveToThread(_thread);
        auto qo = qobject_cast<Connection *>(connection);
        if (qo)
            qo->moveToThread(_thread);
        _thread->start();
        return connection;
    }

    ConnectionRouter *router() const;

private:
    ConnectionRouter *_router = nullptr;
    QThread *_thread          = nullptr;
};
}

#endif // CONNECTIONTHREAD_H
