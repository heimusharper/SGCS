#ifndef IOOBJECT_H
#define IOOBJECT_H
#include "CharMap.h"
#include <list>

namespace tools
{
class IOObject
{
public:
    virtual ~IOObject() = default;

    void addChild(IOObject *child)
    {
        m_childs.push_back(child);
    }
    void removeChild(IOObject *child)
    {
        m_childs.remove(child);
    }
    void writeToChild(const CharMap &data)
    {
        for (auto c : m_childs)
            c->processFromParent(data);
    }
    void setParent(IOObject *parent)
    {
        m_parent = parent;
    }
    void removeParent()
    {
        m_parent = nullptr;
    }
    void writeToParent(const CharMap &data)
    {
        if (m_parent)
            m_parent->processFromChild(data);
    }

    virtual void processFromParent(const CharMap &data) = 0;
    virtual void processFromChild(const CharMap &data)  = 0;

private:
    std::list<IOObject *> m_childs;
    IOObject *m_parent = nullptr;
};
}

#endif // IOOBJECT_H
