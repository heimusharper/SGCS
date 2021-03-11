CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)
PROJECT(PluginIPConnection LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../../bin/plugins )

FIND_PACKAGE(Boost)

# add srcs
SET(SRC_LIST
    PluginIPConnection.cpp
    IPConnection.cpp
    IPConfig.cpp
    IPInterface.cpp
    IPInterfaceUDPClient.cpp
    IPInterfaceUDPServer.cpp
    IPInterfaceTCPClient.cpp
    )
SET(HEADER_LIST
    PluginIPConnection.h
    IPConnection.h
    IPConfig.h
    IPInterface.h
    IPInterfaceUDPClient.h
    IPInterfaceUDPServer.h
    IPInterfaceTCPClient.h
    )

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC SGCS)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIRS})
