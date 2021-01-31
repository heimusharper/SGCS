cmake_minimum_required(VERSION 3.16.0)

project(SGCSApp LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin )

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)

include(FindYaml)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Location REQUIRED)

# add srcs
SET(SRC_LIST
    main.cpp
    )

#create exec
add_executable(${PROJECT_NAME} ${SRC_LIST})

#link
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC SGCS)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC UAV)

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Location)
