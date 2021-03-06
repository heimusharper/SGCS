CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)

PROJECT(SGCS LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../bin )

INCLUDE(GitVersion)
FIND_PACKAGE(Yaml)
FIND_PACKAGE(Boost)

VERSION_WRITE_TO(${CMAKE_CURRENT_SOURCE_DIR})
# add srcs
SET(SRC_LIST
    config/RunConfiguration.cpp
    connection/Connection.cpp
    connection/ConnectionRouter.cpp
    connection/ConnectionThread.cpp
    connection/UavProtocol.cpp
    plugins/ProtocolPlugin.cpp
    plugins/DataSourcePlugin.cpp
    plugins/PluginsLoader.cpp
    plugins/LeafPlugin.cpp
    plugins/LeafInterface.cpp
    GCSStatus.cpp
    GCSRTK.cpp
    )
SET(HEADER_LIST
    config/RunConfiguration.h
    connection/Connection.h
    connection/ConnectionRouter.h
    connection/ConnectionThread.h
    connection/UavProtocol.h
    plugins/PluginInterface.h
    plugins/ProtocolPlugin.h
    plugins/DataSourcePlugin.h
    plugins/PluginsLoader.h
    plugins/LeafPlugin.h
    plugins/LeafInterface.h
    GCSStatus.h
    GCSRTK.h
    version.h
    )

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Yaml_INCLUDE_DIRS})

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST} ${RESOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC UAV)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${Yaml_LIBRARY_DIRS})
