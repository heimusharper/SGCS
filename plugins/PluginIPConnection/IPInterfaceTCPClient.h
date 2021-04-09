#ifndef IPINTERFACETCPCLIENT_H
#define IPINTERFACETCPCLIENT_H
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

class TCPServerClient : public IPChild
{
public:
    TCPServerClient(const std::string &host, uint16_t port) : IPChild(host, port)
    {
    }
};

class IPInterfaceTCPClient : public IPInterface
{
    typedef int TCPSocket;

public:
    IPInterfaceTCPClient();
    virtual ~IPInterfaceTCPClient();

    virtual void start() override final;
    virtual void closeConnection() override final;
    virtual void doConnect(const std::string &host, uint16_t port) override final;

protected:
    void run();

private:
    std::string m_hostName;
    uint16_t m_port;
    const size_t MAX_LINE;
    std::atomic_bool m_reconnect;

    std::mutex m_bufferMutex;
    std::queue<tools::CharMap> m_writeBuffer;

    std::thread *m_thread = nullptr;
    std::atomic_bool m_stopThread;

    IPChild *m_server = nullptr;
};

#endif // IPINTERFACETCPCLIENT_H
