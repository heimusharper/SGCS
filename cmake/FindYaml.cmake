find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIR})

macro(add_yaml TESTNAME)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${YAML_CPP_LIBRARIES})
endmacro()
