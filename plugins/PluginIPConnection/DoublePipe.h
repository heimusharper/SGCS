#ifndef DOUBLEPIPE_H
#define DOUBLEPIPE_H
#include <CharMap.h>
#include <list>

class DoublePipe
{
public:
    DoublePipe();

    void pipeAddChild(DoublePipe *child)
    {
        m_childs.push_back(child);
    }
    void pipewriteToChilds(const tools::CharMap &data)
    {
        for (auto c : m_childs)
            c->pipeProcessFromParent(data);
    }
    void pipeSetParent(DoublePipe *parent)
    {
        m_parent = parent;
    }
    void pipeWriteToParent(const tools::CharMap &data)
    {
        if (m_parent)
            m_parent->pipeProcessFromChild(data);
    }

    virtual void pipeProcessFromParent(const tools::CharMap &data) = 0;
    virtual void pipeProcessFromChild(const tools::CharMap &data)  = 0;

private:
    std::list<DoublePipe *> m_childs;
    DoublePipe *m_parent = nullptr;
};

#endif // DOUBLEPIPE_H
