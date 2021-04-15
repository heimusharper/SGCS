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

bool exitbool = false;
void onExit()
{
    BOOST_LOG_TRIVIAL(warning) << "Close signal";
    exitbool = true;
}

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
class ConnectionFabricImpl : public sgcs::plugin::DataSourcePlugin::ConnectionFabric
{
public:
    virtual ~ConnectionFabricImpl() = default;
    explicit ConnectionFabricImpl(sgcs::plugin::PluginsLoader *loader) : m_loader(loader)
    {
    }
    virtual void onCreate(sgcs::connection::ConnectionThread *thr, sgcs::connection::Connection *instance) override final
    {
        thr->create(instance, m_loader->protocols(), m_loader->leafs());
    }

private:
    sgcs::plugin::PluginsLoader *m_loader = nullptr;
};

int main(int argc, char *argv[])
{
    boost::program_options::options_description description("SGCS commandline");
    description.add_options()("help", "produce help message")("output", "debug output")("trace", "trace output")(
    "auto", "autoconnect")("log", "write log file");

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
    // if (vm.count("output"))
    debugOutput = true;
    // if (vm.count("trace"))
    trace = true;
    std::string logFile;
    if (vm.count("log"))
        logFile = "sample_%N.log";

    initLogger(debugOutput, trace, logFile);
    //
    BOOST_LOG_TRIVIAL(debug) << "processing...";

    if (!RunConfiguration::instance().create("default.yaml"))
    {
        RunConfiguration::instance().get<ApplicationConfiguration>()->setProfile("default");
        RunConfiguration::instance().forceSave();
    }

    sgcs::plugin::PluginsLoader loader;
    std::filesystem::path plugins = std::filesystem::current_path();
    plugins.append("plugins");
    loader.load(plugins);
    sgcs::connection::ConnectionThread thr;
    std::string datasource = RunConfiguration::instance().get<ApplicationConfiguration>()->startDatasource();
    if (!datasource.empty())
    {
        auto dss = loader.datasources();
        for (auto ds : dss)
        {
            BOOST_LOG_TRIVIAL(debug) << "Plugin" << ds->name();
            if (ds->name() == datasource)
            {
                auto instance = ds->instance();
                if (!instance)
                {
                    if (ds->hasConnectionFabric())
                    {
                        ConnectionFabricImpl *cf = new ConnectionFabricImpl(&loader);
                        ds->startConnectionFabric(cf);
                    }
                    else
                        BOOST_LOG_TRIVIAL(error) << "Failed create instance of " << ds->name();
                }
                else
                    thr.create(ds->instance(), loader.protocols(), loader.leafs());
                break;
            }
        }
    }
    else if (vm.count("auto"))
    {
        auto dss = loader.datasources();
        if (!dss.empty())
        {
            thr.create(dss.front()->instance(), loader.protocols(), loader.leafs());
        }
    }
    RunConfiguration::instance().forceSave();
    const int result = std::atexit(onExit);
    while (!exitbool)
    {
        usleep(10);
    }
    return EXIT_SUCCESS;
}
