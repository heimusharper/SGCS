cmake_minimum_required(VERSION 3.16.0)

project(SGCSCmd LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )


find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# core libraries
add_subdirectory(libSGCS)
add_subdirectory(libUAV)
add_subdirectory(plugins)

# add srcs
SET(SRC_LIST
    main.cpp
    )
#SET( SGCS_HEADERS main.cpp )

#create exec
add_executable(${PROJECT_NAME} ${SRC_LIST})

#link
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC SGCS)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC UAV)
