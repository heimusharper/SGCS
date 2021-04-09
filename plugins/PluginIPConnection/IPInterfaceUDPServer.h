#ifndef IPINTERFACEUDPSERVER_H
#define IPINTERFACEUDPSERVER_H
#include "IPInterface.h"
#include <arpa/inet.h>
#include <boost/log/trivial.hpp>
#include <cmath>
#include <fcntl.h>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

typedef int UDPSocket;

class IPInterfaceUDPServer : public IPInterface
{
public:
    IPInterfaceUDPServer();
    virtual ~IPInterfaceUDPServer();

    virtual void start() override final;
    virtual void closeConnection() override final;
    virtual void doConnect(const std::string &host, uint16_t port) override final;

protected:
    void run();

private:
    std::string m_hostName;
    uint16_t m_port;
    const size_t MAX_LINE;

    std::thread *m_thread = nullptr;
    std::atomic_bool m_stopThread;

    std::atomic_bool _dirty = true;

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

    std::map<SockAddr, IPChild *> m_clients;
};

#endif // IPINTERFACEUDPSERVER_H
