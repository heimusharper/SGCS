#ifndef HOME_H
#define HOME_H

#include "UavObject.h"
#include <boost/log/trivial.hpp>
#include <geo/Coords3D.h>

namespace uav
{
class Home : public UavObject<uint8_t>
{
public:
    class Message : public UavTask
    {
    public:
        Message(int target) : UavTask(target), position(geo::Coords3D())
        {
        }
        tools::optional<geo::Coords3D> position;
    };

    Home();
    virtual ~Home();

    void process(Home::Message *message);

    geo::Coords3D position() const;
    void setPosition(geo::Coords3D &&position);

private:
    tools::optional<geo::Coords3D> m_position;
};
}
#endif // HOME_H
