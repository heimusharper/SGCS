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

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Quick REQUIRED)
find_package(Qt5 COMPONENTS SerialPort REQUIRED)
find_package(Qt5 COMPONENTS Qml REQUIRED)
find_package(Qt5 COMPONENTS Location REQUIRED)

version_write_to(${CMAKE_CURRENT_SOURCE_DIR})
# add srcs
SET(SRC_LIST
    SGCS.cpp
    RunConfiguration.cpp
    MainWindow.cpp
    WindowConf.cpp
    SerialConnectionSelectorListModel.cpp
    Connection.cpp
    SerialConnection.cpp
    )
SET(HEADER_LIST
    SGCS.h
    RunConfiguration.h
    MainWindow.h
    WindowConf.h
    SerialConnectionSelectorListModel.h
    Connection.h
    SerialConnection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h
    )

qt5_add_resources(RESOURCES resource.qrc)

# YAML
add_library(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST} ${RESOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_yaml(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Quick)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Qml)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::SerialPort)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Location)
