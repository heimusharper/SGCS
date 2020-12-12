#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H
#include <QtPlugin>

class PluginInterface : public QObject
{
    Q_OBJECT
public:
    PluginInterface()          = default;
    virtual ~PluginInterface() = default;
};

#define PluginInterface_id "ru.sheihar.PluginInterface"
Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_id)
#endif
