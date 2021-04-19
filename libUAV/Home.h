#ifndef UAVHOME_H
#define UAVHOME_H

#include "UavObject.h"
#include <boost/log/trivial.hpp>
#include <geo/Coords3D.h>

namespace uav
{
class Home : public UavObject<uint8_t>
{
public:
    Home();
    virtual ~Home();

    void position(geo::Coords3D &pos);
    void setPosition(const geo::Coords3D &position);

private:
    std::mutex m_positionLock;
    geo::Coords3D m_position;
};
}
#endif // UAVHOME_H
