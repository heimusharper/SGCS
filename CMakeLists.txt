cmake_minimum_required(VERSION 3.16.0)
project(SGCS)
# core libraries
add_subdirectory(libUAV)
add_subdirectory(libSGCS)
add_subdirectory(SGCSApp)
add_subdirectory(plugins/PluginSerialConnection)
add_subdirectory(plugins/PluginMavlink)
