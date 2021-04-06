#include "IPInterfaceTCPClient.h"

IPInterfaceTCPClient::IPInterfaceTCPClient() : IPInterface(), m_hostName(""), m_port(5760), MAX_LINE(1024)
{
    m_reconnect.store(true);
    m_stopThread.store(false);
    m_thread = new std::thread(&IPInterfaceTCPClient::run, this);
}

IPInterfaceTCPClient::~IPInterfaceTCPClient()
{
    m_stopThread.store(true);
    if (m_thread->joinable())
        m_thread->join();
    delete m_thread;
}

void IPInterfaceTCPClient::closeConnection()
{
    m_reconnect.store(false);
}

void IPInterfaceTCPClient::doConnect(const std::string &host, uint16_t port)
{
    m_bufferMutex.lock();
    m_hostName = host;
    m_port     = port;
    m_bufferMutex.unlock();
    m_reconnect.store(true);
}

void IPInterfaceTCPClient::pipeProcessFromParent(const tools::CharMap &data)
{
    m_bufferMutex.lock();
    m_writeBuffer.push(data);
    m_bufferMutex.unlock();
}

void IPInterfaceTCPClient::pipeProcessFromChild(const tools::CharMap &data)
{
    pipeWriteToParent(data);
}

/*std::queue<uint8_t> IPInterfaceTCPClient::readBuffer()
{
    if (!m_readBuffer.empty())
    {
        m_bufferMutex.lock();
        std::queue<uint8_t> copy(m_readBuffer);
        while (!m_readBuffer.empty())
            m_readBuffer.pop();
        m_bufferMutex.unlock();
        return copy;
    }
    return std::queue<uint8_t>();
}
*/

void IPInterfaceTCPClient::run()
{
    TCPSocket sock = -1;

    while (!m_stopThread.load())
    {
        if (m_reconnect.load() && sock >= 0)
        {
            shutdown(sock, SHUT_RDWR);
            sock = -1;
            m_reconnect.store(false);
        }
        if (sock < 0)
        {
            m_bufferMutex.lock();
            // reconnect

            struct sockaddr_in servaddr;
            memset(&servaddr, 0, sizeof(servaddr));
            servaddr.sin_family      = AF_INET; // IPv4
            servaddr.sin_addr.s_addr = INADDR_ANY;
            servaddr.sin_port        = htons(m_port);
            int ret                  = inet_pton(AF_INET, m_hostName.c_str(), &servaddr.sin_addr);
            if (ret == 1)
            {
                sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock >= 0)
                {
                    if (connect(sock, (const struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
                        BOOST_LOG_TRIVIAL(info) << "Connected TCP socket " << m_hostName << ":" << m_port;
                    else
                    {
                        close(sock);
                        sock = -1;
                        BOOST_LOG_TRIVIAL(info) << "Failed bind TCP socket " << m_hostName << ":" << m_port;
                    }
                }
                else
                    BOOST_LOG_TRIVIAL(info) << "Failed to create TCP socket " << m_hostName << ":" << m_port;
            }
            else
                BOOST_LOG_TRIVIAL(info) << "Failed convert address " << m_hostName << " to binary net address";
            m_bufferMutex.unlock();
        }

        if (sock >= 0)
        {
            tools::CharMap readBuffer;
            readBuffer.data    = new char[MAX_LINE];
            readBuffer.size    = MAX_LINE;
            int readBytesCount = recv(sock, (char *)readBuffer.data, MAX_LINE, MSG_DONTWAIT);
            m_bufferMutex.lock();
            if (readBytesCount > 0)
            {
                readBuffer.size = readBytesCount;
                pipeWriteToParent(readBuffer);
            }
            // prepare data to transmit
            while (!m_writeBuffer.empty())
            {
                tools::CharMap cm = m_writeBuffer.front();
                m_writeBuffer.pop();
                send(sock, (const char *)cm.data, cm.size, MSG_CONFIRM);
            }
            m_bufferMutex.unlock();
        }
        if (sock >= 0)
            usleep(100);
        else
            usleep(1000000);
    }
    if (sock > 0)
    {
        close(sock);
    }
}
