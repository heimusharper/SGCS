#ifndef SGCSGCSSTATUS_H
#define SGCSGCSSTATUS_H
#include "GCSRTK.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace sgcs
{
class GCSStatus
{
public:
    static GCSStatus &instance();
    ~GCSStatus();

    boost::uuids::uuid uuid() const;

    GCSRTK *rtk() const;

private:
    GCSStatus();

    boost::uuids::uuid m_uuid;

    GCSRTK *m_rtk;
};
}

#endif // GCSSTATUS_H
