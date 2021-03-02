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
    m_messageGetterThread = new std::thread(&UavProtocol::runTasks, this);
    m_sendThread          = new std::thread(&UavProtocol::runSender, this);
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

void UavProtocol::runTasks()
{
    while (!m_stopThread.load())
    {
        m_mutex.lock();
        if (!m_uavs.empty())
        {
            if (isHasData())
            {
                uav::UavTask *n = message();
                if (!m_uavs.contains(n->targetID))
                    setUAV(n->targetID, new uav::UAV());
                m_uavs.at(n->targetID)->process(std::unique_ptr<uav::UavTask>(std::move(n)));
            }
        }
        m_mutex.unlock();
        usleep(10000);
    }
}

void UavProtocol::runSender()
{
    while (!m_stopThread.load())
    {
        m_sendMutex.lock();
        if (!m_uavs.empty())
        {
            // send
        }
        m_sendMutex.unlock();
        usleep(10000);
    }
}

bool UavProtocol::isHasData() const
{
    return m_hasData.load();
}
uav::UavTask *UavProtocol::message()
{
    m_tasksStoreMutex.lock();
    uav::UavTask *obj = m_tasks.front();
    m_tasks.pop_front();
    if (m_tasks.empty())
        setIsHasData(false);
    m_tasksStoreMutex.unlock();
    return obj;
}

void UavProtocol::setUAV(int id, uav::UAV *uav)
{
    m_uavs.insert({id, uav});
    for (auto handler : _uavCreateHandlers)
        handler->onCreateUav(uav);
}

void UavProtocol::sendMessage(UavSendMessage *message)
{
    m_sendMutex.lock();
    m_send.push_back(message);
    m_sendMutex.unlock();
}

void UavProtocol::addUavCreateHandler(UavProtocol::UavCreateHandler *handler)
{
    _uavCreateHandlers.push_back(handler);
}

}
