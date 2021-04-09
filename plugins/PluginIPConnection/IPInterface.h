#ifndef IPINTERFACE_H
#define IPINTERFACE_H
#include "DoublePipe.h"
#include <CharMap.h>
#include <atomic>
#include <queue>
#include <string>

class IPInterface
{
public:
    class CreateChild
    {
    public:
        virtual void onChild(IPChild *c) = 0;
    };

    IPInterface();
    virtual ~IPInterface() = default;

    virtual void start()                                           = 0;
    virtual void closeConnection()                                 = 0;
    virtual void doConnect(const std::string &host, uint16_t port) = 0;

    void setChildsHandler(CreateChild *c);

protected:
    CreateChild *m_childHandler = nullptr;
};

#endif // IPINTERFACE_H
