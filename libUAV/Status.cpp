#include "Status.h"

namespace uav
{
Status::Status() : m_failures(0)
{
}

void Status::failures(uint16_t &f)
{
    std::lock_guard grd(m_failuresLock);
    f = m_failures;
}

void Status::setFailures(uint16_t failures)
{
    std::lock_guard grd(m_failuresLock);
    if (m_failures == failures)
        return;
    m_failures = failures;
}

}
