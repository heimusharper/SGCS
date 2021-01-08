#include "MainWindow.h"

MainWindow::MainWindow(QObject *parent) : QObject(parent)
{
    _engine = new QQmlApplicationEngine(this);
    _engine->load(QUrl(QStringLiteral("qrc:/ui/MainWindow.qml")));
}

void MainWindow::addEntry(const QByteArray &qml, TargetComponent target)
{
    QString objectName;
    switch (target)
    {
        case TargetComponent::DRAWER_LIST:
            objectName = "drawerButtons";
            break;
        case TargetComponent::ROOT:
            objectName = "root";
            break;
        case TargetComponent::STACK:
            objectName = "stack";
            break;
        default:
            break;
    }
    if (!objectName.isEmpty())
    {
        QQuickWindow *window = qobject_cast<QQuickWindow *>(_engine->rootObjects().first());
        if (window)
        {
            QQuickItem *entry = window->findChild<QQuickItem *>(objectName);
            if (entry)
            {
                qDebug() << "BASE " << _engine->baseUrl();
                QQmlComponent rect1 = QQmlComponent(_engine, entry);
                rect1.setData(qml, _engine->baseUrl());
                QQuickItem *rect1Instance = qobject_cast<QQuickItem *>(rect1.create(_engine->rootContext()));
                QQmlEngine::setObjectOwnership(rect1Instance, QQmlEngine::CppOwnership);
                rect1Instance->setParentItem(entry);
                // rect1Instance->setParent(window);
                return;
            }
        }
    }
}

QQmlApplicationEngine *MainWindow::engine() const
{
    return _engine;
}
