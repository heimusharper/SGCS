#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <RunConfiguration.h>
#include <SGCS.h>
#include <UAV.h>
#include <plugins/PluginInterface.h>
#include <ui/MainWindow.h>

using namespace std;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    if (!RunConfiguration::instance().create("default.yaml"))
    {
        RunConfiguration::instance().get<ApplicationConfiguration>()->setProfile("default");
        RunConfiguration::instance().forceSave();
    }

    // QCoreApplication::addLibraryPath("./");
    PluginLoader::instance().load(QDir("plugins"));

    MainWindow w;
    //    SGCS sgcs;
    //    UAV uav;
    return app.exec();
}
