#include "DoublePipe.h"

IPChild::IPChild()
{
}

void IPChild::processFromParent(const tools::CharMap &)
{
    // without parent
}

void IPChild::processFromChild(const tools::CharMap &data)
{
    std::lock_guard grd(m_bufferMutex);
    m_writeBuffer.push(data);
}
