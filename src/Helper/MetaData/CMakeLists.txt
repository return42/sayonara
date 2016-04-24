PROJECT(say_helper_metadata)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
	MetaData.cpp
	Artist.cpp
	Album.cpp
	LibraryItem.cpp
	MetaDataList.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Core
)
