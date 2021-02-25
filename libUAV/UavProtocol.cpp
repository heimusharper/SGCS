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
#include "UavProtocol.h"

namespace uav
{
UavProtocol::UavProtocol()
{
    m_stopThread.store(false);
    m_hasData.store(false);
}

UavProtocol::~UavProtocol()
{
    m_stopThread.store(true);
    if (m_messageGetterThread)
    {
        if (m_messageGetterThread->joinable())
            m_messageGetterThread->join();
        delete m_messageGetterThread;
    }
}

std::vector<uint8_t> UavProtocol::hello() const
{
    return std::vector<uint8_t>();
}

void UavProtocol::setIsHasData(bool l)
{
    m_hasData.store(l);
}

void UavProtocol::run()
{
    while (!m_stopThread.load())
    {
        m_mutex.lock();
        if (m_uav)
        {
            if (isHasData())
            {
                uav::UavMessage *n = message();
                m_uav->process(n);
            }
        }
        m_mutex.unlock();
        usleep(10000);
    }
}

bool UavProtocol::isHasData() const
{
    return m_hasData.load();
}
uav::UavMessage *UavProtocol::message()
{
    m_messageStoreMutex.lock();
    uav::UavMessage *obj = m_messages.front();
    m_messages.pop_front();
    if (m_messages.empty())
        setIsHasData(false);
    m_messageStoreMutex.unlock();
    return obj;
}

void UavProtocol::setUAV(UAV *uav)
{
    if (m_uav)
        return;
    m_uav                 = uav;
    m_messageGetterThread = new std::thread(&UavProtocol::run, this);
    onSetUAV();
}

}
