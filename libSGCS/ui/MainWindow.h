#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickView>

class MainWindow : public QObject
{
    Q_OBJECT

public:
    enum class TargetComponent
    {
        DRAWER_LIST,
        ROOT,
        STACK
    };

    explicit MainWindow(QObject *parent = nullptr);

    void addEntry(const QByteArray &qml, TargetComponent target);

    QQmlApplicationEngine *engine() const;

private:
    QQmlApplicationEngine *_engine = nullptr;
signals:
};

#endif // MAINWINDOW_H
