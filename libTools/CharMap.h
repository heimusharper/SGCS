#ifndef CHARMAP_H
#define CHARMAP_H
#include <cstring>
#include <uchar.h>

namespace tools
{
class CharMap
{
public:
    CharMap() = default;
    CharMap(const CharMap &m);
    CharMap(CharMap *) = delete;
    ~CharMap();
    char *data  = nullptr;
    size_t size = 0;
};
}
#endif // CHARMAP_H
