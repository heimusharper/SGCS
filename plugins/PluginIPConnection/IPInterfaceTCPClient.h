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

class IPInterfaceTCPClient : public IPInterface
{
    typedef int TCPSocket;

public:
    IPInterfaceTCPClient();
    virtual ~IPInterfaceTCPClient();

    virtual void close() override final;
    virtual void doConnect(const std::string &host, uint16_t port) override final;

    virtual void pipeProcessFromParent(const tools::CharMap &data) override final;
    virtual void pipeProcessFromChild(const tools::CharMap &data) override final;

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
};

#endif // IPINTERFACETCPCLIENT_H
