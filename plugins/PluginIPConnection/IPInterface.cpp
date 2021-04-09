#include "IPInterface.h"

IPInterface::IPInterface()
{
}

void IPInterface::setChildsHandler(IPInterface::CreateChild *c)
{
    m_childHandler = c;
}
