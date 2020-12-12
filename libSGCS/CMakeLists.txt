cmake_minimum_required(VERSION 3.16.0)

project(SGCS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin )
set(CMAKE_AUTOMOC ON)

# add srcs
SET(SRC_LIST
    SGCS.cpp
    RunConfiguration.cpp
    )
SET(HEADER_LIST
    SGCS.h
    RunConfiguration.h
    plugins/PluginInterface.h
    )


find_package(Qt6 COMPONENTS Core REQUIRED)
# YAML
find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST})

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC ${YAML_CPP_LIBRARIES})
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
