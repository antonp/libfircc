# cmake build file.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ircclient)

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
    MESSAGE(FATAL_ERROR "Building standalone is not properly configured yet")

    INCLUDE_DIRECTORIES(anpcode.git/source/defs)
    INCLUDE_DIRECTORIES(anpcode.git/source/common)
    INCLUDE_DIRECTORIES(anpcode.git/source/pfindep)

    ADD_SUBDIRECTORY(anpcode.git/source)
ELSE()
    # Inherit includes from parent instead, and ont care about anpcode
ENDIF()

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

INCLUDE_DIRECTORIES(network cache pluginmanager)
ADD_LIBRARY(fircc SHARED ${ircclient_sources})
TARGET_LINK_LIBRARIES(fircc anpcode)
##### ADD_LIBRARY(anpcode ${COMMON_SRCS})

# pfindep handled by sub make list
#ADD_SUBDIRECTORY(pfindep)

