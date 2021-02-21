#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H
#include <string>

namespace sgcs
{
namespace plugin
{
class PluginInterface
{
public:
    PluginInterface()
    {
    }
    virtual ~PluginInterface() = default;

    virtual std::string name() const = 0;
};
}
}

#endif // PLUGININTERFACE_H
