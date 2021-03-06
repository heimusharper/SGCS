CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)
PROJECT(UAV LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../bin )

FIND_PACKAGE(Boost)

# add srcs
SET(SRC_LIST
    UAV.cpp
    UavMessage.cpp
    UavObject.cpp
    AHRS.cpp
    GPS.cpp
    Position.cpp
    Home.cpp
    Power.cpp
    Speed.cpp
    Mission.cpp
    Status.cpp
    Calibration.cpp
    )
SET(HEADER_LIST
    UAV.h
    UavMessage.h
    UavObject.h
    AHRS.h
    GPS.h
    Position.h
    Home.h
    Power.h
    Speed.h
    Mission.h
    Status.h
    Calibration.h
    )

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC Tools)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIRS})
TARGET_INCLUDE_DIRECTORIES (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
