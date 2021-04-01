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
#ifndef UAVMISSION_H
#define UAVMISSION_H
#include "UavObject.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <geo/Coords3D.h>
#include <vector>

namespace uav
{
class MissionObject
{
public:
    virtual ~MissionObject() = default;
};

class MOPoint : public MissionObject
{
public:
    MOPoint(const geo::Coords3D &coord);
    geo::Coords3D position() const;
    void setPosition(const geo::Coords3D &position);

private:
    geo::Coords3D m_position;
};

class MissionPath
{
public:
    MissionPath();

    boost::uuids::uuid uuid() const;
    boost::uuids::uuid touchUuid() const;

    std::vector<const MissionObject *> objs() const;
    void add(MissionObject *obj);

private:
    void touch();

    std::vector<const MissionObject *> m_objs;
    boost::uuids::uuid m_uuid;
    boost::uuids::uuid m_touchUuid;
};

class Mission : public UavObject<uint16_t>
{
public:
    Mission();
    virtual ~Mission();

    uint maxPatchsCount() const;
    void setMaxPatchsCount(const uint &maxPatchsCount);

    class OnChangeMissionCallback
    {
    public:
        virtual void onUpdatePatchs()                  = 0;
        virtual void onUpdateItem(int patch, int item) = 0;
    };

    // path work

    void addPatch(MissionPath &&mp);
    const MissionPath &patch(int at) const;
    int size() const;

    // callback
    void addCallback(OnChangeMissionCallback *cb);
    void removeCallback(OnChangeMissionCallback *cb);

private:
    std::vector<MissionPath> m_paths;

    uint m_maxPatchsCount = 1;

    // callback
    std::list<OnChangeMissionCallback *> m_callbacks;
};
}
#endif // UAVMISSION_H
