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

#include <IOObject.h>
#include <UAV.h>
#include <UavMessage.h>
#include <atomic>
#include <boost/log/trivial.hpp>
#include <concepts>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

namespace sgcs
{
namespace connection
{
class UavProtocol : public tools::IOObject
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
    virtual tools::CharMap hello() const;
    virtual std::string name() const = 0;

    void sendMessage(uav::UavSendMessage *message);

    void addUavCreateHandler(sgcs::connection::UavProtocol::UavCreateHandler *handler);

    bool isValid() const;

    void startMessaging();

protected:
    void requestToSend();

    virtual void setUAV(int id, uav::UAV *uav);
    void insertMessage(uav::UavTask *message);

    std::list<uav::UavSendMessage *> m_send;
    std::map<int, uav::UAV *> m_uavs;
    std::mutex m_mutex;
    std::atomic_bool m_valid;

    std::mutex m_sendMutex;
    std::atomic_bool m_sendTickStop;
    std::thread *m_sendTick;
    std::atomic_bool m_readyToSend;

private:
    std::list<sgcs::connection::UavProtocol::UavCreateHandler *> _uavCreateHandlers;
};
}
}
#endif // UAVPROTOCOL_H
