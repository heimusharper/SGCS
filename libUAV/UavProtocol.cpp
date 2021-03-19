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
    m_readyMessages.store(false);
    m_stopThread.store(false);
    m_hasData.store(false);
    m_messageGetterThread = new std::thread(&UavProtocol::runTasks, this);
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
    if (!m_readyMessages.load() && l)
        m_readyMessages.store(true);
}

void UavProtocol::runTasks()
{
    while (!m_stopThread.load())
    {
        m_mutex.lock();
        if (isHasData())
        {
            uav::UavTask *n = message();
            if (!m_uavs.contains(n->targetID))
                setUAV(n->targetID, new uav::UAV());
            m_uavs.at(n->targetID)->process(std::unique_ptr<uav::UavTask>(std::move(n)));
        }
        m_mutex.unlock();
        usleep(1000);
    }
}

bool UavProtocol::isHasData() const
{
    return m_hasData.load();
}

bool UavProtocol::isReadyMessages() const
{
    return m_readyMessages.load();
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
    BOOST_LOG_TRIVIAL(debug) << "NEW UAV" << id;
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
    for (auto uav : m_uavs)
        handler->onCreateUav(uav.second);
    _uavCreateHandlers.push_back(handler);
}

UavSendMessage *UavProtocol::next()
{
    uav::UavSendMessage *msg = nullptr;
    m_sendMutex.lock();
    if (!m_send.empty())
    {
        msg = m_send.front();
        m_send.pop_front();
        // send
    }
    m_sendMutex.unlock();
    return msg;
}

}
