cmake_minimum_required(VERSION 3.16.0)

project(PluginSerialConnection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../bin/plugins )
set(CMAKE_AUTOMOC ON)


# add srcs
SET(SRC_LIST
    PluginSerialConnection.cpp
    )
SET(HEADER_LIST
    PluginSerialConnection.h
    )

find_package(Qt5 COMPONENTS Core REQUIRED)

qt5_add_resources(RESOURCES serialConnectionResource.qrc)

add_library(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST} ${RESOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC SGCS)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
