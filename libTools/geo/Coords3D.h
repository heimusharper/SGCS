#ifndef COORDS3D_H
#define COORDS3D_H
#include <cmath>
#include <concepts>
#include <numeric>
namespace geo
{
template <class T>
requires std::floating_point<T>

class Coords
{
public:
    Coords() : m_lat(NAN), m_lon(NAN), m_alt(NAN)
    {
    }
    Coords(T lat, T lon, T alt = NAN) : m_lat(lat), m_lon(lon), m_alt(alt)
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

    void setAlt(T a)
    {
        m_alt = a;
    }

    bool operator==(const Coords<T> &coord) const
    {
        return std::abs(coord.lat() - m_lat) < 0.000001 && std::abs(coord.lon() - m_lon) < 0.000001 &&
        std::abs(coord.alt() - m_alt) < 0.01;
    }

    bool valid() const
    {
        return (!std::isnan(m_alt) || !std::isnan(m_lat) || !std::isnan(m_lon));
    }

private:
    T m_lat;
    T m_lon;
    T m_alt;
};

typedef Coords<double> Coords3D;
typedef Coords<float> Coords3F;

}

#endif // COORDS3D_H
