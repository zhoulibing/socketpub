cmake_minimum_required(VERSION 3.4)
PROJECT(socket)

#SET(CMAKE_C_COMPILER g++)
#SET(CMAKE_CXX_COMPILER g++)
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -g")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpermissive")

ADD_DEFINITIONS(
        -DVOS_WINNT
)

INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/h
)

SET(SOURCE_FILES
        source/tcpsocket.c
        source/tcpsocketserver.c
        source/tcpsocketclient.c
        source/ossstub.c
        source/fsm.c)

ADD_LIBRARY(socket ${SOURCE_FILES})

#TARGET_LINK_LIBRARIES(socket pthread)
