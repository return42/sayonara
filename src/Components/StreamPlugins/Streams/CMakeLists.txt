CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_comp_streamplugins_streams)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCES
	AbstractStreamHandler.cpp
	StreamHandlerPodcasts.cpp
	StreamHandlerStreams.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_helper
	say_helper_parser
	say_comp_playlist
)
