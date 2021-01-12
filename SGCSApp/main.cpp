#include <MainWindow.h>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QSerialPortInfo>
#include <RunConfiguration.h>
#include <SGCS.h>
#include <UAV.h>

using namespace std;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/logo.png"));

    QSerialPortInfo::availablePorts();
    if (!RunConfiguration::instance().create("default.yaml"))
    {
        RunConfiguration::instance().get<ApplicationConfiguration>()->setProfile("default");
        RunConfiguration::instance().forceSave();
    }

    MainWindow w;
    return app.exec();
}
