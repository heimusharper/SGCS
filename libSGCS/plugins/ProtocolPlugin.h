#ifndef PROTOCOLPLUGIN_H
#define PROTOCOLPLUGIN_H

#include <QObject>
#include <UavProtocol.h>

namespace sgcs
{
namespace plugin
{
class ProtocolPlugin : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolPlugin(QObject *parent = nullptr);
    virtual ~ProtocolPlugin() = default;

    virtual QString name() const = 0;

    virtual uav::UavProtocol *instance() = 0;
};
}
}
#define ProtocolPluginInterface_iid "ru.heimusharper.SGCS.ProtocolPluginInterface"
Q_DECLARE_INTERFACE(sgcs::plugin::ProtocolPlugin, ProtocolPluginInterface_iid)

#endif // PROTOCOLPLUGIN_H
