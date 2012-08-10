# cmake build file.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(fircc)

SET(fircc_ver_major 0)
SET(fircc_ver_minor 1)
SET(fircc_ver_patch 1)
SET(fircc_ver_concat ${fircc_ver_major}.${fircc_ver_minor}.${fircc_ver_patch})

ENABLE_TESTING()

# defs can be handled from here

# common handled here too?

IF(UNIX)
    IF(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        ADD_DEFINITIONS(-DTARGET_LINUX_32)
    ELSEIF(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        ADD_DEFINITIONS(-DTARGET_LINUX_64)
    ENDIF(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
ELSEIF(WIN32)
        MESSAGE(FATAL_ERROR "WIN32 not configured")
ENDIF(UNIX)

# Are we building standalone or as a subdir of another project?
IF(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    INCLUDE_DIRECTORIES(anpcode.git/include
                        include)

    ADD_SUBDIRECTORY(anpcode.git)
ELSE()
    # Inherit includes from parent instead, and don't care about anpcode
ENDIF()

ADD_SUBDIRECTORY(samples/consoleapp)
ADD_SUBDIRECTORY(tests)

SET(ircclient_sources network/messagesender.cpp
                      network/network.cpp
                      network/networkfactory.cpp
                      network/tcpconnection.cpp
                      network/tokenizer.cpp
                      cache/channelcache.cpp
                      cache/networkcache.cpp
                      cache/userinfo.cpp
                      cache/utilities.cpp
                      pluginmanager/plugin.cpp
                      pluginmanager/pluginmanager.cpp)

ADD_LIBRARY(fircc SHARED ${ircclient_sources})
TARGET_LINK_LIBRARIES(fircc anpcode)
SET_TARGET_PROPERTIES(fircc
                      PROPERTIES
                      COMPILE_FLAGS "-fPIC"
                      VERSION ${fircc_ver_concat}
                      SOVERSION ${fircc_ver_major})

INSTALL(TARGETS fircc
        DESTINATION lib${LIB_SUFFIX}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

INSTALL(FILES include/fircc/channelcache.h
              include/fircc/inetwork.h
              include/fircc/messageprefix.h
              include/fircc/networkcache.h
              include/fircc/networkcache_userinterface.h
              include/fircc/networkeventdispatchers.h
              include/fircc/networkevents.h
              include/fircc/networkeventsubscribers.h
              include/fircc/networkfactory.h
              include/fircc/pluginmanager.h
        DESTINATION include/fircc
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

# ---- CPack ----
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Manages connections to irc networks")
SET(CPACK_PACKAGE_VENDOR "Anton Petersson")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "${fircc_ver_major}")
SET(CPACK_PACKAGE_VERSION_MINOR "${fircc_ver_minor}")
SET(CPACK_PACKAGE_VERSION_PATCH "${fircc_ver_patch}")

INCLUDE(CPack)

