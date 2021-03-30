#include "IPInterfaceUDPServer.h"

IPInterfaceUDPServer::IPInterfaceUDPServer() : IPInterface(), m_hostName(""), m_port(15760), MAX_LINE(1024)
{
    _dirty.store(true);
    m_stopThread.store(false);
    m_thread = new std::thread(&IPInterfaceUDPServer::run, this);
}

IPInterfaceUDPServer::~IPInterfaceUDPServer()
{
    m_stopThread.store(true);
    m_thread->join();
    delete m_thread;
}

void IPInterfaceUDPServer::closeConnection()
{
    m_bufferMutex.lock();
    m_bufferMutex.unlock();
}

void IPInterfaceUDPServer::doConnect(const std::string &host, uint16_t port)
{
    m_bufferMutex.lock();
    m_hostName = host;
    m_port     = port;
    m_bufferMutex.unlock();
    _dirty.store(true);
}

void IPInterfaceUDPServer::pipeProcessFromParent(const tools::CharMap &data)
{
    m_bufferMutex.lock();
    m_writeBuffer.push(data);
    m_bufferMutex.unlock();
}

void IPInterfaceUDPServer::pipeProcessFromChild(const tools::CharMap &data)
{
    pipeWriteToParent(data);
}
/*
std::queue<uint8_t> IPInterfaceUDPServer::readBuffer()
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

void IPInterfaceUDPServer::writeBuffer(std::queue<uint8_t> &data)
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
}*/

void IPInterfaceUDPServer::run()
{
    UDPSocket sock = -1;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(m_hostName.c_str(), &servaddr.sin_addr);

    std::vector<struct sockaddr_in> clients;
    while (!m_stopThread.load())
    {
        // m_bufferMutex.lock();
        if (sock <= 0)
        {
            // reconnect
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            if (sock > 0)
            {
                servaddr.sin_port = htons(m_port);
                if (bind(sock, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
                {
                    shutdown(sock, SHUT_RD);
                    sock = -1;
                    BOOST_LOG_TRIVIAL(info) << "Failed bind UDP connection " << m_hostName << ":" << m_port;
                }
                else
                {
                    BOOST_LOG_TRIVIAL(info) << "Done bind server " << m_port;
                }
            }
            else
            {
                BOOST_LOG_TRIVIAL(info) << "Failed to create UDP connection " << m_hostName << ":" << m_port;
                sock = -1;
            }
        }
        else
        {
            if (_dirty.load())
            {
                _dirty.store(false);
                close(sock);
                sock = -1;
                continue;
            }
            struct sockaddr_in cliaddr;
            memset(&cliaddr, 0, sizeof(cliaddr));
            socklen_t len = sizeof(cliaddr); // len is value/resuslt

            tools::CharMap readBuffer;
            readBuffer.data = new char[MAX_LINE];
            readBuffer.size = MAX_LINE;
            int n = recvfrom(sock, (char *)readBuffer.data, MAX_LINE, MSG_DONTWAIT, (struct sockaddr *)&cliaddr, &len);
            m_bufferMutex.lock();
            if (n > 0)
            {
                // BOOST_LOG_TRIVIAL(info) << "RECEIVEFROM " << n;
                readBuffer.size = n;
                pipeWriteToParent(readBuffer);
            }
            // to readed buffer
            // prepare data to transmit
            while (!m_writeBuffer.empty())
            {
                tools::CharMap cm = m_writeBuffer.front();
                m_writeBuffer.pop();
                if (cm.size > 0)
                {
                    for (sockaddr_in client : clients)
                    {
                        const size_t l = sizeof(client);
                        // BOOST_LOG_TRIVIAL(info) << "SENDTO";
                        sendto(sock, (const char *)cm.data, cm.size, MSG_DONTWAIT | MSG_NOSIGNAL, (const struct sockaddr *)&client, l);
                    }
                }
            }
            m_bufferMutex.unlock();

            // service
            // search exists clients
            bool has = false;
            for (size_t i = 0; i < clients.size(); i++)
                if (clients.at(i).sin_addr.s_addr == cliaddr.sin_addr.s_addr && clients.at(i).sin_port == cliaddr.sin_port)
                    has = true;
            if (!has)
            { // client not found, add
                clients.push_back(cliaddr);
                // BOOST_LOG_TRIVIAL(debug) << "NEW User " << (int)cliaddr.sin_port << cliaddr.sin_addr.s_addr;
            }
        }
        usleep((sock <= 0) ? 1000000 : 100);
    }
}
