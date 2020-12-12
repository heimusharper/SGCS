#include "MainWindow.h"

MainWindow::MainWindow(QObject *parent) : QObject(parent)
{
    qmlRegisterSingletonType<PluginLoaderListModel>("Ru.SGCS", 1, 0, "PluginLoaderListModel", PluginLoaderListModel::singletonProvider);
    engine.load(QUrl(QStringLiteral("qrc:/ui/MainWindow.qml")));
}
