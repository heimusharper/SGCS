#include "Home.h"

namespace uav
{
Home::Home() : UavObject(), m_position(geo::Coords3D())
{
}

Home::~Home()
{
}

void Home::position(geo::Coords3D &pos)
{
    std::lock_guard grd(m_positionLock);
    pos = m_position;
}

void Home::setPosition(const geo::Coords3D &position)
{
    std::lock_guard grd(m_positionLock);
    if (m_position == position)
        return;
    m_position = position;
    // BOOST_LOG_TRIVIAL(info)
    //<< "New home position " << m_position.get().lat() << " " << m_position.get().lon() << " " << m_position.get().alt();
}
}
