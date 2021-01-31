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
#ifndef CONNECTIONROUTER_H
#define CONNECTIONROUTER_H

#include "../plugins/ProtocolPlugin.h"
#include "Connection.h"
#include <UavProtocol.h>
#include <boost/circular_buffer.hpp>

namespace sgcs
{
namespace connection
{
class ConnectionRouter : public QObject
{
    Q_OBJECT
public:
    ConnectionRouter(Connection *connection, const QList<plugin::ProtocolPlugin *> &protos, QObject *parent = nullptr);
    ~ConnectionRouter();

public slots:

    void run();

private slots:

    void watch();
    void onReceive(const QByteArray &data);

private:
    QTimer *m_watcher = nullptr;
    QList<plugin::ProtocolPlugin *> m_protos;

    Connection *m_connection     = nullptr;
    uav::UavProtocol *m_protocol = nullptr;

    boost::circular_buffer<char> m_buffer;

signals:

    void readyProtocol(const uav::UavProtocol *protocol);
};
}
}
#endif // CONNECTIONROUTER_H
