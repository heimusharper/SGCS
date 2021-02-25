#ifndef COORDS3D_H
#define COORDS3D_H
#include <cmath>

namespace geo
{
template <class T>
class Coords3D
{
public:
    Coords3D() : m_valid(false), m_lat(0), m_lon(0), m_alt(0)
    {
    }
    Coords3D(T lat, T lon, T alt = 0) : m_valid(true), m_lat(lat), m_lon(lon), m_alt(alt)
    {
    }

    void set(T lat, T lon, T alt = 0)
    {
        m_lat = lat;
        m_lon = lon;
        m_alt = alt;
    }

    T lat() const
    {
        return m_lat;
    }
    T lon() const
    {
        return m_lon;
    }
    T alt() const
    {
        return m_alt;
    }

    bool operator==(const Coords3D<T> &coord) const
    {
        return std::abs(coord.lat() - m_lat) < 0.000001 && std::abs(coord.lon() - m_lon) < 0.000001 &&
        std::abs(coord.alt() - m_alt) < 0.01;
    }

private:
    bool m_valid = false;
    T m_lat;
    T m_lon;
    T m_alt;
};
}

#endif // COORDS3D_H
