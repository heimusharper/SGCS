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
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

template <class T>
concept is_UavMessage = requires(T *a)
{
    {
        dynamic_cast<uav::UavTask *>(a) != nullptr
    }
    ->std::convertible_to<bool>;
};

namespace uav
{
class UavProtocol
{
public:
    class UavCreateHandler
    {
    public:
        UavCreateHandler()
        {
        }
        virtual ~UavCreateHandler() = default;

        virtual void onCreateUav(uav::UAV *uav) = 0;
    };
    explicit UavProtocol();
    virtual ~UavProtocol();
    virtual std::vector<uint8_t> hello() const;

    virtual std::string name() const                          = 0;
    virtual void onReceived(const std::vector<uint8_t> &data) = 0;

    bool isHasData() const;
    uav::UavTask *message();

    void sendMessage(uav::UavSendMessage *message);

    void addUavCreateHandler(uav::UavProtocol::UavCreateHandler *handler);

protected:
    virtual void setUAV(int id, uav::UAV *uav);

    void setIsHasData(bool l);

    template <is_UavMessage T>
    void insertMessage(uav::UavTask *message)
    {
        m_tasksStoreMutex.lock();
        m_tasks.remove_if([](uav::UavTask *msg) { return (dynamic_cast<T *>(msg) != nullptr); });
        m_tasks.push_back(message);
        m_tasksStoreMutex.unlock();
        setIsHasData(true);
    }

    std::map<int, uav::UAV *> m_uavs;

    std::thread *m_messageGetterThread = nullptr;
    std::mutex m_mutex;

private:
    std::list<UavCreateHandler *> _uavCreateHandlers;

    std::list<uav::UavTask *> m_tasks;
    std::mutex m_tasksStoreMutex;
    std::atomic_bool m_stopThread;
    void runTasks();

    // send
    std::list<uav::UavSendMessage *> m_send;
    std::thread *m_sendThread = nullptr;
    std::mutex m_sendMutex;
    void runSender();

private:
    std::atomic_bool m_hasData;
};
}
#endif // UAVPROTOCOL_H
