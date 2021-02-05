FIND_PATH(Boost_INCLUDE_DIR
    NAMES
        circular_buffer.hpp
    PATHS
        /usr/local/include/boost
        /usr/local/include
        /usr/include/boost
        /usr/include)
FIND_LIBRARY(Boost_LIBRARY_DIR
    NAMES
        boost_container
    PATHS
        /usr/local/lib64
        /usr/local/lib
        /usr/lib
        /lib
)
SET(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
SET(Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIR} )

SET(Boost_FOUND "NO")
IF (Boost_INCLUDE_DIRS AND Boost_LIBRARY_DIRS)
    SET(Boost_FOUND "YES")
ENDIF()
