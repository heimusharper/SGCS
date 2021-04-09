#include "IPInterfaceUDPServer.h"

IPInterfaceUDPServer::IPInterfaceUDPServer() : IPInterface(), m_hostName(""), m_port(15760), MAX_LINE(1024)
{
}

IPInterfaceUDPServer::~IPInterfaceUDPServer()
{
    m_stopThread.store(true);
    m_thread->join();
    delete m_thread;
}

void IPInterfaceUDPServer::start()
{
    _dirty.store(true);
    m_stopThread.store(false);
    m_thread = new std::thread(&IPInterfaceUDPServer::run, this);
}

void IPInterfaceUDPServer::closeConnection()
{
}

void IPInterfaceUDPServer::doConnect(const std::string &host, uint16_t port)
{
    m_hostName = host;
    m_port     = port;
    _dirty.store(true);
}

void IPInterfaceUDPServer::run()
{
    UDPSocket sock = -1;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(m_hostName.c_str(), &servaddr.sin_addr);

    while (!m_stopThread.load())
    {
        if (sock <= 0)
        {
            // reconnect
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            if (sock > 0)
            {
                servaddr.sin_port = htons(m_port);
                if (bind(sock, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
                {
                    close(sock);
                    sock = -1;
                    BOOST_LOG_TRIVIAL(info) << "Failed bind UDP connection " << m_hostName << ":" << m_port;
                }
                else
                    BOOST_LOG_TRIVIAL(info) << "Done bind server " << m_port;
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
            if (n > 0)
            {
                if (!m_clients.contains(cliaddr))
                {
                    BOOST_LOG_TRIVIAL(info) << "New client " << cliaddr.sin_port;
                    // create new
                    IPChild *c = new IPChild;
                    m_clients.insert(std::pair(cliaddr, c));
                    if (m_childHandler)
                        m_childHandler->onChild(c);
                }
                readBuffer.size = n;
                m_clients.at(cliaddr)->writeToChild(readBuffer);
            }

            // to readed buffer
            // prepare data to transmit
            for (const std::pair<SockAddr, IPChild *> &value : m_clients)
            {
                const size_t l = sizeof(value.first.addr);
                std::lock_guard grd(value.second->m_bufferMutex);
                while (!value.second->m_writeBuffer.empty())
                {
                    tools::CharMap cm = value.second->m_writeBuffer.front();
                    value.second->m_writeBuffer.pop();
                    if (cm.size > 0)
                    {
                        sendto(sock,
                               (const char *)cm.data,
                               cm.size,
                               MSG_DONTWAIT | MSG_NOSIGNAL,
                               (const struct sockaddr *)&value.first.addr,
                               l);
                    }
                }
            }
        }
        usleep((sock <= 0) ? 1000000 : 20);
    }
}
