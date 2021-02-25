#ifndef LEAFPLUGIN_H
#define LEAFPLUGIN_H
#include "LeafInterface.h"
#include "PluginInterface.h"

namespace sgcs
{
namespace plugin
{
class LeafPlugin : public PluginInterface
{
public:
    LeafPlugin();
    virtual ~LeafPlugin() = default;

    virtual gcs::LeafInterface *leaf() = 0;
};
}
}

#endif // LEAFPLUGIN_H
