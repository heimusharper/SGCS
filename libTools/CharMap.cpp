#include "CharMap.h"

namespace tools
{
CharMap::CharMap(const CharMap &m)
{
    data = new char[m.size];
    memcpy(data, m.data, m.size);
    size = m.size;
}

CharMap::~CharMap()
{
    if (size > 0)
        delete[] data;
}

}
