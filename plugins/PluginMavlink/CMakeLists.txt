cmake_minimum_required(VERSION 3.16.0)
project(PluginMavlink LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../bin/plugins )
set(CMAKE_AUTOMOC ON)

# begin mavlink
FIND_PATH(MAVLINK_INCLUDE_DIR
    NAMES
        protocol.h
    PATHS
    ${PROJECT_SOURCE_DIR}/../../thirdparty/c_library_v2)

IF (NOT MAVLINK_INCLUDE_DIR)
    execute_process(COMMAND
        python2 mavlink/pymavlink/tools/mavgen.py --output c_library_v2 --lang C --wire-protocol 2.0 mavlink/message_definitions/v1.0/ardupilotmega.xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../../thirdparty
        )
ENDIF()

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



find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Location REQUIRED)

# add srcs
SET(SRC_LIST
    PluginMavlink.cpp
    MavlinkProtocol.cpp
    )
SET(HEADER_LIST
    PluginMavlink.h
    MavlinkProtocol.h
    )
SET(OTHER_FILES
    meta.json
)

add_library(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST} ${OTHER_FILES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC SGCS)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC UAV)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)

#target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Location)
