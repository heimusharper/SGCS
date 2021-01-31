#ifndef DATASOURCEPLUGIN_H
#define DATASOURCEPLUGIN_H

#include "../connection/Connection.h"
#include <QObject>

namespace sgcs
{
namespace plugin
{
class DataSourcePlugin : public QObject
{
    Q_OBJECT
public:
    explicit DataSourcePlugin(QObject *parent = nullptr);
    virtual ~DataSourcePlugin() = default;

    virtual QString name() const = 0;

    virtual sgcs::connection::Connection *instance() = 0;
signals:
};
}
}
#define DataSourcePluginInterface_iid "ru.heimusharper.SGCS.DataSourcePluginInterface"
Q_DECLARE_INTERFACE(sgcs::plugin::DataSourcePlugin, DataSourcePluginInterface_iid)

#endif // DATASOURCEPLUGIN_H
