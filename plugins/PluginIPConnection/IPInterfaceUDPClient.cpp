#include "IPInterfaceUDPClient.h"

IPInterfaceUDPClient::IPInterfaceUDPClient() : IPInterface(), m_hostName(""), m_port(14560), MAX_LINE(1024)
{
    m_reconnect.store(true);
    m_stopThread.store(false);
    m_thread = new std::thread(&IPInterfaceUDPClient::run, this);
}

IPInterfaceUDPClient::~IPInterfaceUDPClient()
{
    m_stopThread.store(true);
    m_thread->join();
    delete m_thread;
}

void IPInterfaceUDPClient::close()
{
    m_reconnect.store(false);
}

void IPInterfaceUDPClient::doConnect(const std::string &host, uint16_t port)
{
    m_bufferMutex.lock();
    m_hostName = host;
    m_port     = port;
    m_bufferMutex.unlock();
    m_reconnect.store(true);
}

std::queue<uint8_t> IPInterfaceUDPClient::readBuffer()
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

void IPInterfaceUDPClient::writeBuffer(std::queue<uint8_t> &data)
{
    if (!data.empty())
    {
        m_bufferMutex.lock();
        while (!data.empty())
        {
            m_writeBuffer.push(data.front());
            data.pop();
        }
        m_bufferMutex.unlock();
    }
}

void IPInterfaceUDPClient::run()
{
    UDPSocket sock = -1;
    struct sockaddr_in servaddr;
    {
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family      = AF_INET; // IPv4
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        inet_aton(m_hostName.c_str(), &servaddr.sin_addr);
        BOOST_LOG_TRIVIAL(info) << "SET HOST " << m_hostName;
    }
    struct sockaddr_in clientaddr;
    {
        memset(&clientaddr, 0, sizeof(servaddr));
        clientaddr.sin_family      = AF_INET; // IPv4
        clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        clientaddr.sin_port        = htons(m_port); // LINUX
    }

    while (!m_stopThread.load())
    {
        {
            // connection
            const uint16_t listenPort = 11250 + (rand() / RAND_MAX * 1000 - 500);
            servaddr.sin_port         = htons(listenPort);

            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (sock > 0)
            {
                // listen1
                if (bind(sock, (const struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0)
                {
                    shutdown(sock, SHUT_RD);
                    sock = -1;
                    BOOST_LOG_TRIVIAL(info) << "Failed bind UDP listen connection " << m_hostName << ":" << listenPort;
                }
                BOOST_LOG_TRIVIAL(info) << "Listen on " << m_hostName << ":" << listenPort;
                struct timeval tv;
                tv.tv_sec  = 1;
                tv.tv_usec = 0;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
                m_reconnect.store(false);
            }
            else
                BOOST_LOG_TRIVIAL(info) << "Failed to create UDP connection " << m_hostName << ":" << m_port;
        }
        while (!m_stopThread.load() && !m_reconnect.load())
        {
            // RW
            socklen_t len = sizeof(servaddr);
            char readBuffer[MAX_LINE];
            int n = recvfrom(sock, (char *)readBuffer, MAX_LINE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);

            const size_t WRITE_SIZE = std::min(MAX_LINE, m_writeBuffer.size());
            char writeBuffer[WRITE_SIZE];
            m_bufferMutex.lock();
            // to readed buffer
            for (int i = 0; i < n; i++)
                m_readBuffer.push(readBuffer[i]);
            // prepare data to transmit
            for (size_t i = 0; i < WRITE_SIZE; i++)
            {
                writeBuffer[i] = m_writeBuffer.front();
                m_writeBuffer.pop();
            }
            m_bufferMutex.unlock();
            if (WRITE_SIZE > 0)
            {
                BOOST_LOG_TRIVIAL(info) << "::::>" << WRITE_SIZE << " " << sock;
                sendto(sock, (const char *)writeBuffer, WRITE_SIZE, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
            }
            usleep(1000);
        }
        usleep(100000);
    }
}
