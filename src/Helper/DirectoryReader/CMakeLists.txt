PROJECT(say_helper_directory_reader)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(SOURCES
	DirectoryReader.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_helper_tagging
	say_helper_parser
)
