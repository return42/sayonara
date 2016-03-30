PROJECT(say_comp_library)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

ADD_SUBDIRECTORY(Importer)

SET(SOURCES
    AbstractLibrary.cpp
    LocalLibrary.cpp
    threads/ReloadThread.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_tagedit
	say_comp_engine
	say_comp_playlist
	say_comp_library_importer
	say_helper_directory_reader
)

