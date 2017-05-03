
PROJECT(say_comp_library)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(SOURCES
    AbstractLibrary.cpp
    LocalLibrary.cpp
	LibraryManager.cpp

    Importer/ImportCache.cpp
    Importer/CachingThread.cpp
    Importer/CopyThread.cpp
    Importer/LibraryImporter.cpp

    Threads/ReloadThread.cpp
    Threads/IndexDirectoriesThread.cpp
    Threads/FileSystemWatcher.cpp
    Threads/UpdateDatesThread.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_tagedit
    say_comp_playlist
)

