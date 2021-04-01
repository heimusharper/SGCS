#include "Mission.h"

namespace uav
{
Mission::Mission()
{
}

Mission::~Mission()
{
}

uint Mission::maxPatchsCount() const
{
    return m_maxPatchsCount;
}

void Mission::setMaxPatchsCount(const uint &maxPatchsCount)
{
    m_maxPatchsCount = maxPatchsCount;
}

void Mission::addPatch(MissionPath &&mp)
{
    m_paths.push_back(mp);
    if (m_paths.size() > m_maxPatchsCount)
        m_paths.erase(m_paths.begin());

    for (auto x : m_callbacks)
        x->onUpdatePatchs();
}

const MissionPath &Mission::patch(int at) const
{
    return m_paths[at];
}

int Mission::size() const
{
    return m_paths.size();
}

void Mission::addCallback(Mission::OnChangeMissionCallback *cb)
{
    m_callbacks.push_back(cb);
}

void Mission::removeCallback(Mission::OnChangeMissionCallback *cb)
{
    m_callbacks.remove(cb);
}

MissionPath::MissionPath() : m_uuid(boost::uuids::random_generator()())
{
    touch();
}

boost::uuids::uuid MissionPath::uuid() const
{
    return m_uuid;
}

std::vector<const MissionObject *> MissionPath::objs() const
{
    return m_objs;
}

void MissionPath::add(MissionObject *obj)
{
    m_objs.push_back(obj);
}

void MissionPath::touch()
{
    m_touchUuid = boost::uuids::random_generator()();
}

boost::uuids::uuid MissionPath::touchUuid() const
{
    return m_touchUuid;
}

MOPoint::MOPoint(const geo::Coords3D &coord) : MissionObject(), m_position(coord)
{
}

geo::Coords3D MOPoint::position() const
{
    return m_position;
}

void MOPoint::setPosition(const geo::Coords3D &position)
{
    m_position = position;
}

}
