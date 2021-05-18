#ifndef IPINTERFACETCPCLIENT_H
#define IPINTERFACETCPCLIENT_H
#include "DoublePipe.h"
#include "IPInterface.h"
#include <arpa/inet.h>
#include <boost/log/trivial.hpp>
#include <cmath>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

class IPInterfaceTCPClient : public IPInterface
{
    typedef int TCPSocket;

public:
    IPInterfaceTCPClient();
    virtual ~IPInterfaceTCPClient();

    virtual void start() override final;
    virtual void closeConnection() override final;
    virtual void doConnect(const std::string &host, uint16_t port) override final;

    void setChildsHandler(CreateChild *c);

protected:
    void run();

private:
    struct SockAddr
    {
        SockAddr(struct sockaddr_in a) : addr(a)
        {
        }
        bool operator<(const SockAddr &a) const
        {
            return this->addr.sin_addr.s_addr < a.addr.sin_addr.s_addr || this->addr.sin_port < a.addr.sin_port;
        }
        struct sockaddr_in addr;
    };

    std::string m_hostName;
    uint16_t m_port;
    const size_t MAX_LINE;
    std::atomic_bool _dirty = true;

    std::thread *m_thread = nullptr;
    std::atomic_bool m_stopThread;

    IPChild *m_server = nullptr;
};

#endif // IPINTERFACETCPCLIENT_H
