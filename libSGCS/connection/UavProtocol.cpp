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

namespace sgcs
{
namespace connection
{
UavProtocol::UavProtocol() : tools::IOObject()
{
    m_send.insert(std::pair(uav::UavSendMessage::Priority::HIGHT, new std::vector<uav::UavSendMessage *>()));
    m_send.insert(std::pair(uav::UavSendMessage::Priority::NORMAL, new std::vector<uav::UavSendMessage *>()));
    m_send.insert(std::pair(uav::UavSendMessage::Priority::LOW, new std::vector<uav::UavSendMessage *>()));

    m_send[uav::UavSendMessage::Priority::HIGHT]->reserve(50);
    m_send[uav::UavSendMessage::Priority::NORMAL]->reserve(50);
    m_send[uav::UavSendMessage::Priority::LOW]->reserve(50);

    m_valid.store(false);
    m_sendTickStop.store(false);
}

UavProtocol::~UavProtocol()
{
    m_sendTickStop.store(true);
    if (m_sendTick)
    {
        if (m_sendTick->joinable())
            m_sendTick->join();
        delete m_sendTick;
    }
    delete m_send[uav::UavSendMessage::Priority::HIGHT];
    delete m_send[uav::UavSendMessage::Priority::NORMAL];
    delete m_send[uav::UavSendMessage::Priority::LOW];
}

tools::CharMap UavProtocol::hello() const
{
    return tools::CharMap();
}

void UavProtocol::setUAV(int id, uav::UAV *uav)
{
    BOOST_LOG_TRIVIAL(info) << "NEW UAV" << id;
    m_uavs.insert({id, uav});
    for (auto handler : _uavCreateHandlers)
        handler->onCreateUav(uav);
}

void UavProtocol::insertMessage(uav::UavTask *message)
{
    m_valid.store(true);
    m_mutex.lock();
    if (!m_uavs.contains(message->targetID))
        setUAV(message->targetID, new uav::UAV());

    m_uavs.at(message->targetID)->process(std::unique_ptr<uav::UavTask>(message));
    m_mutex.unlock();
}

void UavProtocol::sendMessage(uav::UavSendMessage *message)
{
    // if (tosend.compare(m, uav::UavSendMessage::CompareMode::HARD)) // TODO: Check exists
    m_send[message->priority()]->push_back(message);
    // requestToSend();
}

void UavProtocol::addUavCreateHandler(sgcs::connection::UavProtocol::UavCreateHandler *handler)
{
    for (auto uav : m_uavs)
        handler->onCreateUav(uav.second);
    _uavCreateHandlers.push_back(handler);
}

bool UavProtocol::isValid() const
{
    return m_valid.load();
}

void UavProtocol::startMessaging()
{
    m_sendTick = new std::thread([this]() {
        while (!m_sendTickStop.load())
        {
            if (requestToSend())
                usleep(10000);
            else
                usleep(100);
        }
    });
}

void UavProtocol::processFromParent(const tools::CharMap &data)
{
    std::list<uav::UavSendMessage *> messages = createUAVMesage(data);
    if (!messages.empty())
    {
        while (!messages.empty())
        {
            auto m = messages.front();
            for (auto tosendvec : m_send)
            {
                bool dobrk = false;
                for (size_t i = 0; i < tosendvec.second->size(); i++)
                {
                    auto tosend = tosendvec.second->at(i);
                    if (tosend->compare(m, uav::UavSendMessage::CompareMode::LIGHT))
                    {
                        tosendvec.second->erase(tosendvec.second->begin() + i);
                        dobrk = true;
                        break;
                    }
                }
                if (dobrk)
                    break;
            }
            messages.pop_front();
            delete m;
        }
    }
}

bool UavProtocol::requestToSend()
{
    if (!requestToSend(m_send[uav::UavSendMessage::Priority::HIGHT]))
        if (!requestToSend(m_send[uav::UavSendMessage::Priority::NORMAL]))
            if (!requestToSend(m_send[uav::UavSendMessage::Priority::LOW]))
                return false;
    return true;
}

bool UavProtocol::requestToSend(std::vector<uav::UavSendMessage *> *fromlist)
{
    if (!fromlist->empty())
    {
        if (m_sendMutex.try_lock())
        {
            bool writed = false;
            // BOOST_LOG_TRIVIAL(info) << "START " << fromlist->size();
            for (size_t i = 0; i < fromlist->size(); i++)
            {
                auto message = fromlist->at(i);
                // BOOST_LOG_TRIVIAL(info)
                // << "    MESSAGE " << message->isReadyInterval() << " " << message->isReadyToDelete() << " " << message;
                if (!writed && message->isReadyInterval())
                {
                    // BOOST_LOG_TRIVIAL(info) << "        SEND...";
                    writeToParent(message->pack());
                    message->touch();
                    writed = true;
                }
                if (message->isReadyToDelete())
                {
                    // BOOST_LOG_TRIVIAL(info) << "        REMOVE...";
                    fromlist->erase(fromlist->begin() + i);
                    delete message;
                }
            }
            m_sendMutex.unlock();
            return writed;
        }
    }
    return false;
}
}
}
