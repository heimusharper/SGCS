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

#ifndef MAVLINKPROTOCOL_H
#define MAVLINKPROTOCOL_H

#include "ardupilotmega/mavlink.h"
#include <UavProtocol.h>
#include <atomic>
#include <boost/log/trivial.hpp>
#include <mutex>
#include <queue>
#include <thread>

class MavlinkProtocol : public uav::UavProtocol
{
public:
    explicit MavlinkProtocol();
    ~MavlinkProtocol();

    virtual std::string name() const override;
    virtual boost::container::vector<uint8_t> hello() const override;

    virtual void onReceived(const boost::container::vector<uint8_t> &data) override;

private:
    void run();

private:
    bool check(char c, mavlink_message_t *msg);

    static boost::container::vector<uint8_t> packMessage(mavlink_message_t *msg);

    const int DIFFERENT_CHANNEL     = 1;
    std::atomic_bool _isCheckAPM    = false;
    std::atomic_bool _waitForSignal = true;

    std::queue<mavlink_message_t> _mavlinkMessages;
    std::mutex _mavlinkStoreMutex;

    std::atomic_bool _stopThread;
    std::thread *_dataProcessorThread = nullptr;
    std::queue<boost::container::vector<uint8_t>> _dataTasks;
    std::mutex _dataTaskMutex;
};

#endif // MAVLINKPROTOCOL_H
