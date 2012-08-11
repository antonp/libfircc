# cmake build file.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(fircc_tests)

# defs can be handled from here

IF(UNIX)
    IF(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        ADD_DEFINITIONS(-DTARGET_LINUX_32)
    ELSEIF(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        ADD_DEFINITIONS(-DTARGET_LINUX_64)
    ENDIF(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
ELSEIF(WIN32)
        MESSAGE(FATAL_ERROR "WIN32 not configured")
ENDIF(UNIX)

ENABLE_TESTING()

# Eventdispatcher tests
ADD_EXECUTABLE(test_eventdispatcher eventdispatcher.cpp)
TARGET_LINK_LIBRARIES(test_eventdispatcher fircc pthread dl pcrecpp)
SET_TARGET_PROPERTIES(test_eventdispatcher
                      PROPERTIES
                      COMPILE_FLAGS "-Wall"
                      VERSION ${fircc_ver_concat})

ADD_TEST(dontcrash test_eventdispatcher)
SET_TESTS_PROPERTIES(dontcrash PROPERTIES PASS_REGULAR_EXPRESSION
                    "NewNetwork event received")

# Pluginmanager tests
ADD_LIBRARY(testimpl_plugin1 SHARED testimpl_plugin1.cpp)
SET_TARGET_PROPERTIES(testimpl_plugin1
                      PROPERTIES
                      COMPILE_FLAGS "-fPIC -Wall"
                      VERSION ${fircc_ver_concat}
                      SOVERSION ${fircc_ver_major})

ADD_LIBRARY(testimpl_plugin2 SHARED testimpl_plugin2.cpp)
SET_TARGET_PROPERTIES(testimpl_plugin2
                      PROPERTIES
                      COMPILE_FLAGS "-fPIC -Wall"
                      VERSION ${fircc_ver_concat}
                      SOVERSION ${fircc_ver_major})

ADD_EXECUTABLE(test_reloadplugins test_reloadplugins.cpp)
TARGET_LINK_LIBRARIES(test_reloadplugins fircc pthread dl pcrecpp)
SET_TARGET_PROPERTIES(test_reloadplugins
                      PROPERTIES
                      COMPILE_FLAGS "-Wall"
                      VERSION ${fircc_ver_concat})

ADD_TEST(reload test_reloadplugins)
SET_TESTS_PROPERTIES(reload PROPERTIES PASS_REGULAR_EXPRESSION
                     "p1 i\np1 d\np1 i\np2 i\np1 d\np2 d")

