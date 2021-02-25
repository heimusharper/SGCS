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
    // IO
    virtual std::queue<uint8_t> readBuffer() override final;
    virtual void writeBuffer(std::queue<uint8_t> &data) override final;

protected:
    void run();

private:
    enum class ConnectionStates : char
    {
        DISCONNECTED = 0x0,
        CONNECTED    = 0x1
    };

    std::mutex m_bufferMutex;
    std::queue<uint8_t> m_readBuffer;
    std::queue<uint8_t> m_writeBuffer;

    std::thread *m_thread = nullptr;
    std::atomic_bool m_stopThread;
    std::atomic_char m_targetState;

    std::string m_hostName;
    uint16_t m_port;
    const size_t MAX_LINE = 1024;
};

#endif // IPINTERFACEUDPCLIENT_H