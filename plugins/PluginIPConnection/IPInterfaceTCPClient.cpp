#include "IPInterfaceTCPClient.h"

IPInterfaceTCPClient::IPInterfaceTCPClient() : IPInterface(), m_hostName(""), m_port(5760), MAX_LINE(1024)
{
}

IPInterfaceTCPClient::~IPInterfaceTCPClient()
{
    m_stopThread.store(true);
    if (m_thread->joinable())
        m_thread->join();
    delete m_thread;
}

void IPInterfaceTCPClient::start()
{
    _dirty.store(true);
    m_stopThread.store(false);
    m_thread = new std::thread(&IPInterfaceTCPClient::run, this);
}

void IPInterfaceTCPClient::closeConnection()
{
}

void IPInterfaceTCPClient::doConnect(const std::string &host, uint16_t port)
{
    m_hostName = host;
    m_port     = port;
    _dirty.store(true);
}

void IPInterfaceTCPClient::setChildsHandler(IPInterface::CreateChild *c)
{
    IPInterface::setChildsHandler(c);
    if (m_server)
        c->onChild(m_server);
}

void IPInterfaceTCPClient::run()
{
    m_server       = new IPChild;
    TCPSocket sock = -1;
    if (m_childHandler)
        m_childHandler->onChild(m_server);

    while (!m_stopThread.load())
    {
        if (sock <= 0)
        {
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

            tools::CharMap readBuffer;
            readBuffer.data    = new char[MAX_LINE];
            readBuffer.size    = MAX_LINE;
            int readBytesCount = recv(sock, (char *)readBuffer.data, MAX_LINE, MSG_DONTWAIT);
            if (readBytesCount > 0)
            {
                readBuffer.size = readBytesCount;
                m_server->writeToChild(readBuffer);
            }
            // prepare data to transmit
            {
                std::lock_guard grd(m_server->m_bufferMutex);
                while (!m_server->m_writeBuffer.empty())
                {
                    tools::CharMap cm = m_server->m_writeBuffer.front();
                    m_server->m_writeBuffer.pop();
                    if (cm.size > 0)
                    {
                        send(sock, (const char *)cm.data, cm.size, MSG_DONTWAIT | MSG_NOSIGNAL);
                    }
                }
            }
        }
        usleep((sock <= 0) ? 1000000 : 5000);
    }
    if (sock > 0)
    {
        close(sock);
    }
}
