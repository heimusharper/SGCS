#ifndef IPINTERFACE_H
#define IPINTERFACE_H
#include <atomic>
#include <queue>
#include <string>

class IPInterface
{
public:
    IPInterface();
    virtual ~IPInterface() = default;

    virtual void close()                                           = 0;
    virtual void doConnect(const std::string &host, uint16_t port) = 0;

    // IO
    virtual std::queue<uint8_t> readBuffer()            = 0;
    virtual void writeBuffer(std::queue<uint8_t> &data) = 0;
};

#endif // IPINTERFACE_H
