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
#include <QDir>
#include <QGuiApplication>
#include <QSerialPortInfo>
#include <UAV.h>
#include <config/RunConfiguration.h>
#include <connection/ConnectionThread.h>
#include <plugins/PluginsLoader.h>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/program_options.hpp>

void initLogger(bool output, bool trace, const std::string &file)
{
    if (!output)
        boost::log::core::get()->set_filter((trace) ? boost::log::trivial::severity >= boost::log::trivial::trace
                                                    : boost::log::trivial::severity >= boost::log::trivial::info);

    if (!file.empty())
    {
        boost::log::add_file_log(boost::log::keywords::file_name     = file,
                                 boost::log::keywords::rotation_size = 10 * 1024 * 1024,
                                 boost::log::keywords::time_based_rotation =
                                 boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
                                 boost::log::keywords::format = "[%TimeStamp%]: %Message%");

        boost::log::core::get()->set_filter((trace) ? boost::log::trivial::severity >= boost::log::trivial::trace
                                                    : boost::log::trivial::severity >= boost::log::trivial::info);
    }
}
int main(int argc, char *argv[])
{
    boost::program_options::options_description description("SGCS commandline");
    description.add_options()("help", "produce help message")("output", "debug output")("trace", "trace output")("auto", "autoconnect");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);
    boost::program_options::notify(vm);

    bool debugOutput = false;
    bool trace       = false;

    if (vm.count("help"))
    {
        initLogger(true, false, "");
        BOOST_LOG_TRIVIAL(debug) << description << "\n";
        return 1;
    }
    if (vm.count("output"))
        debugOutput = true;
    if (vm.count("trace"))
        trace = true;
    std::string logFile;
    if (vm.count("log"))
        logFile = "sample_%N.log";

    initLogger(debugOutput, trace, logFile);
    //
    BOOST_LOG_TRIVIAL(debug) << "processing...";

    QGuiApplication app(argc, argv);
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
    std::string datasource = RunConfiguration::instance().get<ApplicationConfiguration>()->startDatasource();
    if (!datasource.empty())
    {
        auto dss = loader.datasources();
        for (auto ds : dss)
        {
            if (ds->name().compare(QString::fromStdString(datasource)) == 0)
            {
                thr.create(ds->instance(), loader.protocols());
                break;
            }
        }
    }
    else if (vm.count("auto"))
    {
        BOOST_LOG_TRIVIAL(debug) << "DS" << datasource;
        auto dss = loader.datasources();
        if (!dss.empty())
        {
            BOOST_LOG_TRIVIAL(debug) << "DS" << datasource;
            thr.create(dss.front()->instance(), loader.protocols());
        }
    }
    RunConfiguration::instance().forceSave();
    return app.exec();
}
