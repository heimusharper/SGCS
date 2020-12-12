#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "plugins/PluginLoader.h"
#include <QObject>
#include <QQmlApplicationEngine>

class MainWindow : public QObject
{
    Q_OBJECT

public:
    explicit MainWindow(QObject *parent = nullptr);

private:
    QQmlApplicationEngine engine;
signals:
};

#endif // MAINWINDOW_H
