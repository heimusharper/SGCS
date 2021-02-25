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
#ifndef UAVPROTOCOL_H
#define UAVPROTOCOL_H

#include "UavMessage.h"
#include <UAV.h>
#include <UavMessage.h>
#include <atomic>
#include <boost/container/vector.hpp>
#include <list>
#include <mutex>

namespace uav
{
class UavProtocol
{
public:
    explicit UavProtocol();
    virtual ~UavProtocol() = default;
    virtual boost::container::vector<uint8_t> hello() const;

    virtual std::string name() const                                       = 0;
    virtual void onReceived(const boost::container::vector<uint8_t> &data) = 0;

    bool isHasData() const;
    uav::UavMessage *message();

protected:
    void setIsHasData(bool l);

    template <class T>
    void insertMessage(uav::UavMessage *message)
    {
        _messageStoreMutex.lock();
        _messages.remove_if([](uav::UavMessage *msg) { return (dynamic_cast<T *>(msg) != nullptr); });
        _messages.push_back(message);
        _messageStoreMutex.unlock();
        setIsHasData(true);
    }

private:
    std::list<uav::UavMessage *> _messages;
    std::mutex _messageStoreMutex;

private:
    std::atomic_bool _hasData;
};
}
#endif // UAVPROTOCOL_H
