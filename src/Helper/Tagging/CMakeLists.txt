CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_helper_tagging)

SET(SOURCES

	Tagging.cpp

	Frames/AbstractFrame.cpp
	Frames/Discnumber.cpp
	Frames/Popularimeter.cpp
	Frames/Cover.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	tag
	say_helper
)
