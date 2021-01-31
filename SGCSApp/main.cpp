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
#include "iostream"
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QSerialPortInfo>
#include <UAV.h>
#include <config/RunConfiguration.h>
#include <connection/ConnectionThread.h>
#include <plugins/PluginsLoader.h>

using namespace std;
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    std::cout << (qUtf8Printable(msg)) << std::endl;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);
    QGuiApplication app(argc, argv);
    qDebug() << "processing...";
    if (!RunConfiguration::instance().create("default.yaml"))
    {
        RunConfiguration::instance().get<ApplicationConfiguration>()->setProfile("default");
        RunConfiguration::instance().forceSave();
    }

    sgcs::plugin::PluginsLoader loader;
    QDir plugins = QDir(app.applicationDirPath());
    plugins.cd("plugins");
    loader.load(plugins);
    sgcs::connection::ConnectionThread thr;
    QString datasource = RunConfiguration::instance().get<ApplicationConfiguration>()->startDatasource();
    if (!datasource.isEmpty())
    {
        auto dss = loader.datasources();
        for (auto ds : dss)
        {
            if (ds->name().compare(datasource) == 0)
            {
                qDebug() << "DS: " << ds->name() << "take";
                thr.create(ds->instance(), loader.protocols());
                break;
            }
            else
            {
                qDebug() << "DS: " << ds->name() << "pass";
            }
        }
    }
    return app.exec();
}
