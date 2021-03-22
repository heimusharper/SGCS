#ifndef IPINTERFACEUDPCLIENT_H
#define IPINTERFACEUDPCLIENT_H
#include "IPInterface.h"
#include <arpa/inet.h>
#include <boost/container/vector.hpp>
#include <boost/log/trivial.hpp>
#include <cmath>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

typedef int UDPSocket;
class IPInterfaceUDPClient : public IPInterface
{
public:
    IPInterfaceUDPClient();
    virtual ~IPInterfaceUDPClient();

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
    std::queue<tools::CharMap> m_readBuffer;
    std::queue<tools::CharMap> m_writeBuffer;

    std::thread *m_thread = nullptr;
    std::atomic_bool m_stopThread;
    std::atomic_char m_targetState;
};

#endif // IPINTERFACEUDPCLIENT_H
