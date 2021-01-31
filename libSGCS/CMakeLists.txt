cmake_minimum_required(VERSION 3.16.0)

project(SGCS LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin )
set(CMAKE_AUTOMOC ON)

include(FindYaml)
include(GitVersion)
include(FindBoost)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS SerialPort REQUIRED)
find_package(Qt5 COMPONENTS Location REQUIRED)

version_write_to(${CMAKE_CURRENT_SOURCE_DIR})
# add srcs
SET(SRC_LIST
    config/RunConfiguration.cpp
    connection/Connection.cpp
    connection/ConnectionRouter.cpp
    connection/ConnectionThread.cpp
    plugins/ProtocolPlugin.cpp
    plugins/DataSourcePlugin.cpp
    plugins/PluginsLoader.cpp
    )
SET(HEADER_LIST
    config/RunConfiguration.h
    connection/Connection.h
    connection/ConnectionRouter.h
    connection/ConnectionThread.h
    plugins/ProtocolPlugin.h
    plugins/DataSourcePlugin.h
    plugins/PluginsLoader.h
    version.h
    )

INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${YAML_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC UAV)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::SerialPort)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Location)

message(${YAML_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${BOOST_LIBRARY_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${YAML_LIBRARY_DIR})
