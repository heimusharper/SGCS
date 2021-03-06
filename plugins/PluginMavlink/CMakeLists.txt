CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)
PROJECT(PluginMavlink LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../../bin/plugins )

FIND_PACKAGE(Boost)
# generate mavlink
EXECUTE_PROCESS(COMMAND
    python2 mavlink/pymavlink/tools/mavgen.py --output c_library_v2 --lang C --wire-protocol 2.0 mavlink/message_definitions/v1.0/all.xml
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../../thirdparty
    )

FIND_PATH(MAVLINK_INCLUDE_DIR
    NAMES
        protocol.h
    PATHS
    ${PROJECT_SOURCE_DIR}/../../thirdparty/c_library_v2)

IF (NOT MAVLINK_INCLUDE_DIR)
    message("Failed build mavlink")
ENDIF()
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../thirdparty/c_library_v2)
# end mavlink

# add srcs
SET(SRC_LIST
    PluginMavlink.cpp
    MavlinkProtocol.cpp
    IAutopilot.cpp
    AutopilotAPMImpl.cpp
    AutopilotPixhawkImpl.cpp
    MavlinkConfig.cpp
    )
SET(HEADER_LIST
    MavlinkHelper.h
    PluginMavlink.h
    MavlinkProtocol.h
    IAutopilot.h
    AutopilotAPMImpl.h
    AutopilotPixhawkImpl.h
    MavlinkConfig.h
    )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC SGCS)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC UAV)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIRS})
