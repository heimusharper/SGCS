#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QPluginLoader>
#include <QQmlApplicationEngine>
#include <RunConfiguration.h>
#include <SGCS.h>
#include <UAV.h>
#include <plugins/PluginInterface.h>

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

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/ui/MainWindow.qml")));

    SGCS sgcs;
    UAV uav;
    QDir dir("plugins");
    for (auto pluginFilename : dir.entryList())
    {
        QString fullName = dir.absoluteFilePath(pluginFilename);
        if (QLibrary::isLibrary(fullName))
        {
            QPluginLoader loader(fullName);
            if (auto instance = loader.instance())
            {
                if (qobject_cast<PluginInterface *>(instance))
                {
                    qDebug() << "Done load lugin" << fullName;
                }
                else
                {
                    qDebug() << "qobject_cast<> returned nullptr";
                }
            }
            else
            {
                qDebug() << loader.errorString();
            }
        }
        else
        {
            qWarning() << "Failed: " << fullName << " is not a library/plugin";
        }
    }
    return app.exec();
}
