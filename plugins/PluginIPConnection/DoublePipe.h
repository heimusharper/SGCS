#ifndef DOUBLEPIPE_H
#define DOUBLEPIPE_H
#include <CharMap.h>
#include <connection/Connection.h>
#include <queue>
#include <string>

class IPChild : public sgcs::connection::Connection
{
public:
    IPChild();
    virtual void processFromParent(const tools::CharMap &)
    {
        // without parent
    }
    virtual void processFromChild(const tools::CharMap &data)
    {
        std::lock_guard grd(m_bufferMutex);
        m_writeBuffer.push(data);
    }

    std::mutex m_bufferMutex;
    std::queue<tools::CharMap> m_writeBuffer;
};
#endif // DOUBLEPIPE_H
