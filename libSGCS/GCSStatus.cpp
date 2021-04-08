#include "GCSStatus.h"

namespace sgcs
{
GCSStatus &GCSStatus::instance()
{
    static GCSStatus s;
    return s;
}

GCSStatus::GCSStatus() : m_uuid(boost::uuids::random_generator()()), m_rtk(new GCSRTK())
{
}
GCSStatus::~GCSStatus()
{
    delete m_rtk;
}

GCSRTK *GCSStatus::rtk() const
{
    return m_rtk;
}

boost::uuids::uuid GCSStatus::uuid() const
{
    return m_uuid;
}
}
