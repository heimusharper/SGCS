#include "IPInterfaceTCPClient.h"

IPInterfaceTCPClient::IPInterfaceTCPClient() : IPInterface(), m_hostName(""), m_port(5760), MAX_LINE(1024)
{
    m_stopThread.store(false);
    m_targetState.store((char)ConnectionStates::DISCONNECTED);

    m_thread = new std::thread(&IPInterfaceTCPClient::run, this);
}

IPInterfaceTCPClient::~IPInterfaceTCPClient()
{
    m_stopThread.store(true);
    if (m_thread->joinable())
        m_thread->join();
    delete m_thread;
}

void IPInterfaceTCPClient::close()
{
    m_bufferMutex.lock();
    m_targetState.store((char)ConnectionStates::DISCONNECTED);
    m_bufferMutex.unlock();
}

void IPInterfaceTCPClient::doConnect(const std::string &host, uint16_t port)
{
    m_bufferMutex.lock();
    m_hostName = host;
    m_port     = port;
    m_targetState.store((char)ConnectionStates::CONNECTED);
    m_bufferMutex.unlock();
}

std::queue<uint8_t> IPInterfaceTCPClient::readBuffer()
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

void IPInterfaceTCPClient::writeBuffer(std::queue<uint8_t> &data)
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

void IPInterfaceTCPClient::run()
{
    TCPSocket sock = -1;
    std::string nowHostName;
    uint16_t nowPort = 0;

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
        if (m_targetState.load() == (char)ConnectionStates::CONNECTED && (nowHostName != m_hostName || nowPort != m_port))
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
                // reconnect
                if (sock >= 0)
                {
                    shutdown(sock, SHUT_RDWR);
                }
                sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock >= 0)
                {
                    if (connect(sock, (const struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
                    {
                        BOOST_LOG_TRIVIAL(info) << "Connected TCP socket " << m_hostName << ":" << m_port;
                        nowHostName = m_hostName;
                        nowPort     = m_port;
                    }
                    else
                    {
                        shutdown(sock, SHUT_RDWR);
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
            char readBuffer[MAX_LINE];
            int readBytesCount = recv(sock, (char *)readBuffer, MAX_LINE, MSG_DONTWAIT);

            const size_t WRITE_SIZE = std::min(MAX_LINE, m_writeBuffer.size());
            char writeBuffer[WRITE_SIZE];
            m_bufferMutex.lock();
            // to readed buffer
            for (int i = 0; i < readBytesCount; i++)
                m_readBuffer.push(readBuffer[i]);
            // prepare data to transmit
            for (size_t i = 0; i < WRITE_SIZE; i++)
            {
                writeBuffer[i] = m_writeBuffer.front();
                m_writeBuffer.pop();
            }
            m_bufferMutex.unlock();
            if (WRITE_SIZE > 0)
                send(sock, (const char *)writeBuffer, WRITE_SIZE, MSG_CONFIRM);
        }
        if (sock >= 0)
            usleep(10000);
        else
            usleep(1000000);
    }
}
