FIND_PATH(Yaml_INCLUDE_DIR
    NAMES
        yaml-cpp/yaml.h
    PATHS
        /usr/local/include/boost
        /usr/local/include
        /usr/include/boost
        /usr/include)
FIND_LIBRARY(Yaml_LIBRARY_DIR
    NAMES
        yaml-cpp
    PATHS
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib64
        /usr/local/lib
        /usr/lib
        /lib
)

SET(Yaml_INCLUDE_DIRS ${Yaml_INCLUDE_DIR})
SET(Yaml_LIBRARY_DIRS ${Yaml_LIBRARY_DIR} )

SET(Yaml_FOUND "NO")
IF (Yaml_INCLUDE_DIRS AND Yaml_LIBRARY_DIRS)
    SET(Yaml_FOUND "YES")
ENDIF()
