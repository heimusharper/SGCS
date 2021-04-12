#include "Status.h"

namespace uav
{
Status::Status()
{
}

uint16_t Status::failures() const
{
    return m_failures;
}

void Status::setFailures(const uint16_t &failures)
{
    if (m_failures == failures)
        return;
    m_failures = failures;
}
void Status::process(Status::Message *message)
{
}

}
