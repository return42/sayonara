PROJECT(say_comp_library)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCES

    AbstractLibrary.cpp
    LibraryImporter.cpp
    LocalLibrary.cpp
    threads/ReloadThread.cpp
    threads/ImportCachingThread.cpp
    threads/ImportCopyThread.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_tagedit
	say_comp_engine
	say_comp_playlist
    say_helper_directory_reader
)

