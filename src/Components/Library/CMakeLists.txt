PROJECT(say_comp_library)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(SOURCES
    AbstractLibrary.cpp
    LocalLibrary.cpp
    threads/ReloadThread.cpp
    threads/IndexDirectoriesThread.cpp
    threads/FileSystemWatcher.cpp
    Importer/ImportCache.cpp
    Importer/CachingThread.cpp
    Importer/CopyThread.cpp
    Importer/LibraryImporter.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_tagedit
	say_comp_playlist
)

