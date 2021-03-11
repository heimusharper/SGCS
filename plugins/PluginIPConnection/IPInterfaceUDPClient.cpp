#include "IPInterfaceUDPClient.h"

IPInterfaceUDPClient::IPInterfaceUDPClient() : IPInterface()
{
    m_stopThread.store(false);
    m_targetState.store((char)ConnectionStates::DISCONNECTED);

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
    m_bufferMutex.lock();
    m_targetState.store((char)ConnectionStates::DISCONNECTED);
    m_bufferMutex.unlock();
}

void IPInterfaceUDPClient::doConnect(const std::string &host, uint16_t port)
{
    m_bufferMutex.lock();
    m_hostName = host;
    m_port     = port;
    m_targetState.store((char)ConnectionStates::CONNECTED);
    m_bufferMutex.unlock();
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
    std::string nowHostName;
    uint16_t nowPort = 0;
    struct sockaddr_in clientaddr;

    boost::container::vector<struct sockaddr_in> clients;

    while (!m_stopThread.load())
    {
        char nowState = (char)((sock == 0) ? ConnectionStates::DISCONNECTED : ConnectionStates::CONNECTED);
        if (nowState != (char)m_targetState.load())
        {
            // state changed
            if (m_targetState.load() == (char)ConnectionStates::DISCONNECTED)
            {
                sock = -1;
            }
            else
            {
                nowPort = 0;
            }
        }
        // m_bufferMutex.lock();
        if (m_targetState.load() == (char)ConnectionStates::CONNECTED && (nowHostName != m_hostName || nowPort != m_port))
        {
            // reconnect
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (sock > 0)
            {
                // listen
                struct sockaddr_in servaddr;
                memset(&servaddr, 0, sizeof(servaddr));
                servaddr.sin_family      = AF_INET; // IPv4
                servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
                servaddr.sin_port        = htons(14400);
                if (bind(sock, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
                {
                    shutdown(sock, SHUT_RD);
                    sock = -1;
                    BOOST_LOG_TRIVIAL(info) << "Failed bind UDP listen connection " << m_hostName << ":" << m_port;
                }
                BOOST_LOG_TRIVIAL(info) << "Listen on " << m_hostName << ":" << m_port;

                memset(&clientaddr, 0, sizeof(servaddr));
                clientaddr.sin_family      = AF_INET; // IPv4
                clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);
                clientaddr.sin_port        = htons(m_port);
                if (inet_aton(m_hostName.c_str(), &clientaddr.sin_addr) == 0)
                {
                    shutdown(sock, SHUT_RD);
                    sock = -1;
                    BOOST_LOG_TRIVIAL(info) << "Failed bind UDP client connection " << m_hostName << ":" << m_port;
                }
                m_writeBuffer.push('h');
                m_writeBuffer.push('e');
                m_writeBuffer.push('l');
                m_writeBuffer.push('l');
                m_writeBuffer.push('o');
                m_writeBuffer.push('\n');
            }
            else
                BOOST_LOG_TRIVIAL(info) << "Failed to create UDP connection " << m_hostName << ":" << m_port;
        }
        // m_bufferMutex.unlock();

        if (sock >= 0)
        {
            struct sockaddr_in cliaddr;
            memset(&cliaddr, 0, sizeof(cliaddr));
            socklen_t len = sizeof(cliaddr); // len is value/resuslt

            char readBuffer[MAX_LINE];
            int n = recvfrom(sock, (char *)readBuffer, MAX_LINE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);

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
                const size_t l = sizeof(clientaddr);
                BOOST_LOG_TRIVIAL(info) << "::::>" << writeBuffer;
                sendto(sock, (const char *)writeBuffer, WRITE_SIZE, 0, (const struct sockaddr *)&clientaddr, l);
            }

            // service
            // search exists clients
            bool has = false;
            for (int i = 0; i < clients.size(); i++)
                if (clients.at(i).sin_addr.s_addr == cliaddr.sin_addr.s_addr && clients.at(i).sin_port == cliaddr.sin_port)
                    has = true;
            if (!has) // client not found, add
                clients.push_back(cliaddr);
        }
        usleep(1000);
    }
}
