#ifndef IPINTERFACE_H
#define IPINTERFACE_H
#include "IOObject.h"
#include <CharMap.h>
#include <atomic>
#include <queue>
#include <string>

class IPInterface : public tools::IOObject
{
public:
    IPInterface();
    virtual ~IPInterface() = default;

    virtual void close()                                           = 0;
    virtual void doConnect(const std::string &host, uint16_t port) = 0;
};

#endif // IPINTERFACE_H
