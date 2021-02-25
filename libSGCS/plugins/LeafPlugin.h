#ifndef LEAFPLUGIN_H
#define LEAFPLUGIN_H
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
};
}
}

#endif // LEAFPLUGIN_H
