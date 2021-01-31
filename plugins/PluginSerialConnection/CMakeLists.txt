cmake_minimum_required(VERSION 3.16.0)
project(PluginSerialConnection LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../bin/plugins )
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Location REQUIRED)

# add srcs
SET(SRC_LIST
    PluginSerialConnection.cpp
    SerialConnection.cpp
    )
SET(HEADER_LIST
    PluginSerialConnection.h
    SerialConnection.h
    )
SET(OTHER_FILES
    meta.json
)

add_library(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST} ${OTHER_FILES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC SGCS)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)

#target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Location)
