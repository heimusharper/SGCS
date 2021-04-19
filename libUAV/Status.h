#ifndef STATUS_H
#define STATUS_H
#include "UavObject.h"
#include <Optional.h>
#include <cmath>
#include <list>

namespace uav
{
class Status : public UavObject<uint16_t>
{
public:
    enum HAS : uint16_t
    {
        MAG   = 0b1 << 0,
        GPS   = 0b1 << 1,
        ACCEL = 0b1 << 2
    };
    Status();

    void failures(uint16_t &f);
    void setFailures(uint16_t failures);

private:
    std::mutex m_failuresLock;
    uint16_t m_failures;
};
}
#endif // STATUS_H
