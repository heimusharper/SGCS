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
    class Message : public UavTask
    {
    public:
        Message(int target) : UavTask(target), failure(0)
        {
        }
        tools::optional<uint16_t> failure;
    };

    void process(Status::Message *message);
    Status();

    uint16_t failures() const;
    void setFailures(const uint16_t &failures);

private:
    uint16_t m_failures;
};
}
#endif // STATUS_H
