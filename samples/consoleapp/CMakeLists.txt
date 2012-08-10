# cmake build file.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(fircc_sampleapp)

IF(UNIX)
    IF(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        ADD_DEFINITIONS(-DTARGET_LINUX_32)
    ELSEIF(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        ADD_DEFINITIONS(-DTARGET_LINUX_64)
    ENDIF(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
ELSEIF(WIN32)
        MESSAGE(FATAL_ERROR "WIN32 not configured")
ENDIF(UNIX)

ADD_EXECUTABLE(libfircc_sampleapp main.cpp)
TARGET_LINK_LIBRARIES(libfircc_sampleapp anpcode fircc pthread dl pcrecpp)
SET_TARGET_PROPERTIES(libfircc_sampleapp
                      PROPERTIES
                      COMPILE_FLAGS "-fPIC -Wall"
                      VERSION ${fircc_ver_concat})
INSTALL(TARGETS libfircc_sampleapp
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

