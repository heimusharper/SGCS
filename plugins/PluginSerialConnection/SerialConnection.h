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
#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H
#include "SerialConfig.h"
#include <CharMap.h>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <connection/Connection.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/serial.h>
#include <queue>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

typedef int SerialDescriptor;

class SerialConnection : public sgcs::connection::Connection
{
public:
    explicit SerialConnection();
    virtual ~SerialConnection();

    virtual void process(const tools::CharMap &data) override final;
    virtual void processFromChild(const tools::CharMap &data) override final;

    void connectToPort(const std::string &portName, int baudRate);
    void disconnectFromPort();

private:
    void run();

private:
    std::string m_portName;
    uint32_t m_baudRate;
    bool m_autoConnect;
    const size_t MAX_BUFFER_SIZE;
    const size_t MAX_READ_BYTES_SIZE;

    std::queue<tools::CharMap> m_writeBuffer;

    std::atomic_bool m_stop;
    std::atomic_bool m_isDirty;
    std::mutex m_mutex;
    std::thread *m_thread = nullptr;

    std::queue<std::string> m_portNames;
};

#endif // SERIALCONNECTION_H
