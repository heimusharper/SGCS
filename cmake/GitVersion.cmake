# get version info from git
# for example: git describe --tags --long
# returns: v0.1-2-gc0fb956
# major is 0 (tag name)
# minor is 1 (tag name)
# path is 2 (commits in from tag)
# hash is gc0fb956 (last commit short hash)
MACRO(VERSION_WRITE_TO TARGET_PATH)

    EXECUTE_PROCESS( COMMAND
        git describe --tags --long
    WORKING_DIRECTORY ${TARGET_PATH}
    OUTPUT_VARIABLE VERSION_FULL)

    STRING(REGEX REPLACE "v([0-9]+)\.([0-9]+)\-([0-9]+)-([0-9a-z]+)\n"
       "\\1;\\2;\\3;\\4" RESULT_VERSION ${VERSION_FULL})

    LIST (GET RESULT_VERSION 0 VERSION_MAJOR)
    LIST (GET RESULT_VERSION 1 VERSION_MINOR)
    LIST (GET RESULT_VERSION 2 VERSION_PATH)
    LIST (GET RESULT_VERSION 3 VERSION_HASH)
    
    MESSAGE(STATUS "GIT version: ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATH}-${VERSION_HASH}")
    CONFIGURE_FILE(version.h.in ${TARGET_PATH}/version.h)
ENDMACRO()
