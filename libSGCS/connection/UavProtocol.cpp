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
UavProtocol::UavProtocol()
{
    m_valid.store(false);
}

UavProtocol::~UavProtocol()
{
}

tools::CharMap UavProtocol::hello() const
{
    return tools::CharMap();
}

void UavProtocol::setUAV(int id, uav::UAV *uav)
{
    BOOST_LOG_TRIVIAL(debug) << "NEW UAV" << id;
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
    writeToParent(message->pack());
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
}
}
