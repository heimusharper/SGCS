/*
 * This file is part of the SGCS distribution (https://github.com/heimusharper/SGCS).
 * Copyright (c) 2020 Andrey Stepanov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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
