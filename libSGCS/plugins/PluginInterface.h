#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H
#include "../RunConfiguration.h"
#include <QtPlugin>

class PluginInterface : public QObject
{
    Q_OBJECT
public:
    PluginInterface()          = default;
    virtual ~PluginInterface() = default;

    virtual bool load() const    = 0;
    virtual QString name() const = 0;
};

#define PluginInterface_id "ru.sheihar.PluginInterface"
Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_id)
#endif
