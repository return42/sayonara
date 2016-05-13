PROJECT(say_helper_parser)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
	AbstractPlaylistParser.cpp

	M3UParser.cpp
	PLSParser.cpp
	ASXParser.cpp

	PlaylistParser.cpp
	PodcastParser.cpp
	StreamParser.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_covers
	say_helper
)
