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
#include <concepts>
#include <list>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

template <class T>
concept is_UavMessage = requires(T *a)
{
    {
        dynamic_cast<uav::UavMessage *>(a) != nullptr
    }
    ->std::convertible_to<bool>;
};

namespace uav
{
class UavProtocol
{
public:
    explicit UavProtocol();
    virtual ~UavProtocol();
    virtual std::vector<uint8_t> hello() const;

    virtual std::string name() const                          = 0;
    virtual void onReceived(const std::vector<uint8_t> &data) = 0;

    bool isHasData() const;
    uav::UavMessage *message();

    void setUAV(uav::UAV *uav);

protected:
    void setIsHasData(bool l);
    virtual void onSetUAV() = 0;

    template <is_UavMessage T>
    void insertMessage(uav::UavMessage *message)
    {
        m_messageStoreMutex.lock();
        m_messages.remove_if([](uav::UavMessage *msg) { return (dynamic_cast<T *>(msg) != nullptr); });
        m_messages.push_back(message);
        m_messageStoreMutex.unlock();
        setIsHasData(true);
    }

    uav::UAV *m_uav = nullptr;

    std::thread *m_messageGetterThread = nullptr;
    std::mutex m_mutex;

private:
    std::list<uav::UavMessage *> m_messages;
    std::mutex m_messageStoreMutex;
    std::atomic_bool m_stopThread;

    void run();

private:
    std::atomic_bool m_hasData;
};
}
#endif // UAVPROTOCOL_H
