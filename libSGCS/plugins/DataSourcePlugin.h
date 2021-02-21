#ifndef DATASOURCEPLUGIN_H
#define DATASOURCEPLUGIN_H

#include "../connection/Connection.h"
#include "PluginInterface.h"

namespace sgcs
{
namespace plugin
{
class DataSourcePlugin : public PluginInterface
{
public:
    DataSourcePlugin();
    virtual ~DataSourcePlugin() = default;

    virtual sgcs::connection::Connection *instance() = 0;
};
}
}

#endif // DATASOURCEPLUGIN_H
