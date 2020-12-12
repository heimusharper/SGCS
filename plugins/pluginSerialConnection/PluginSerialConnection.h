#ifndef UAV_H
#define UAV_H
#include "plugins/PluginInterface.h"

class PluginSerialConnection : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_id)
    Q_INTERFACES(PluginInterface)

public:
    PluginSerialConnection();
    virtual ~PluginSerialConnection();
};

#endif // SGCS_H
