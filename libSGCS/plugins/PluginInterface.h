#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H
#include "../RunConfiguration.h"
#include "../ui/MainWindow.h"
#include <QtPlugin>

class PluginInterface : public QObject
{
    Q_OBJECT
public:
    PluginInterface()          = default;
    virtual ~PluginInterface() = default;

    virtual bool isLoad() const  = 0;
    virtual QString name() const = 0;
    virtual bool create()        = 0;

    virtual void setWindow(MainWindow *w) final
    {
        if (_window)
            return;
        _window = w;
    }

protected:
    MainWindow *_window = nullptr;
};

#define PluginInterface_id "ru.sheihar.PluginInterface"
Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_id)
#endif
