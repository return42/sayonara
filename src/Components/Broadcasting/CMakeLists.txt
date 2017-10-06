PROJECT(say_comp_broadcasting)

SET(SOURCES
	StreamServer.cpp
	StreamWriter.cpp
	StreamHttpParser.cpp
	StreamDataSender.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_interfaces_raw_sound_receiver
	say_helper
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()
