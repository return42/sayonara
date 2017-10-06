PROJECT(say_comp_streamplugins_streams)

SET(SOURCES
	AbstractStreamHandler.cpp
	StreamHandlerPodcasts.cpp
	StreamHandlerStreams.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
    say_comp_playlist
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

