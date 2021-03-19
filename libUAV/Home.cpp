#include "Home.h"

namespace uav
{
Home::Home() : UavObject(), m_position(geo::Coords3D())
{
}

Home::~Home()
{
}

void Home::process(std::unique_ptr<Home::Message> message)
{
    if (message->position.dirty() && message->position.get().valid())
        setPosition(std::move(message->position.get()));
}

geo::Coords3D Home::position() const
{
    return m_position.get();
}

void Home::setPosition(geo::Coords3D &&position)
{
    if (m_position.get() == position)
        return;
    m_position = position;
    // BOOST_LOG_TRIVIAL(info)
    // << "New home position " << m_position.get().lat() << " " << m_position.get().lon() << " " << m_position.get().alt();
}
}
