#ifndef IPINTERFACE_H
#define IPINTERFACE_H
#include "DoublePipe.h"
#include <CharMap.h>
#include <atomic>
#include <queue>
#include <string>

class IPInterface : public DoublePipe
{
public:
    IPInterface();
    virtual ~IPInterface() = default;

    virtual void closeConnection()                                           = 0;
    virtual void doConnect(const std::string &host, uint16_t port) = 0;
};

#endif // IPINTERFACE_H
