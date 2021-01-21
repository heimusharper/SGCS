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
#include "WindowConf.h"

MainWindow::MainWindow(QObject *parent) : QObject(parent)
{
    qmlRegisterType<SerialConnectionSelectorListModel>("sgcs", 1, 0, "SerialConnectionSelectorListModel");
    qmlRegisterType<UavObject>("sgcs", 1, 0, "UavObject");
    // qRegisterMetaType<UavObject>("UavObject");
    qmlRegisterType<UavFabric>("sgcs", 1, 0, "UavFabric");

    qmlRegisterType<WindowConf>("sgcs", 1, 0, "WindowConf");

    _engine = new QQmlApplicationEngine(this);
    _engine->load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));
}

QIcon MainWindow::icon()
{
    return QIcon(":/logo.png");
}
