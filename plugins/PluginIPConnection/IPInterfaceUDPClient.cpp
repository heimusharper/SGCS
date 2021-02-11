#include "IPInterfaceUDPClient.h"

IPInterfaceUDPClient::IPInterfaceUDPClient()
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
            sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock > 0)
            {
                struct sockaddr_in servaddr;
                memset(&servaddr, 0, sizeof(servaddr));
                servaddr.sin_family      = AF_INET; // IPv4
                servaddr.sin_addr.s_addr = INADDR_ANY;
                servaddr.sin_port        = htons(m_port);
                if (bind(sock, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
                {
                }
                else
                {
                    shutdown(sock, SHUT_RD);
                    sock = -1;
                    BOOST_LOG_TRIVIAL(info) << "Failed bind UDP connection " << m_hostName << m_port;
                }
            }
            else
                BOOST_LOG_TRIVIAL(info) << "Failed to create UDP connection " << m_hostName << m_port;
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
                for (sockaddr_in client : clients)
                {
                    const size_t l = sizeof(client);
                    sendto(sock, (const char *)writeBuffer, WRITE_SIZE, MSG_CONFIRM, (const struct sockaddr *)&client, l);
                }
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
