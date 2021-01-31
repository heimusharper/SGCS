GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)

FIND_PATH(BOOST_INCLUDE_DIR
    NAMES
        circular_buffer.hpp
    PATHS
        /usr/local/include/boost
        /usr/local/include
        /usr/include/boost
        /usr/include)
FIND_LIBRARY(BOOST_LIBRARY_DIR
    NAMES
        libboost_container.so
    PATHS
        /usr/local/lib64
        /usr/local/lib
        /usr/lib
        /lib
)
