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
#ifndef CONNECTION_H
#define CONNECTION_H

#include "UavProtocol.h"
#include <IOObject.h>
#include <memory.h>
#include <vector>

namespace sgcs
{
namespace connection
{
class Connection : public tools::IOObject
{
public:
    explicit Connection();
    virtual ~Connection();
};
}
}
#endif // CONNECTION_H
