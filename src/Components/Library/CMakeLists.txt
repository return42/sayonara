PROJECT(say_comp_library)

SET(SOURCES
    AbstractLibrary.cpp
    LocalLibrary.cpp
    LibraryManager.cpp
    GenreFetcher.cpp

    Importer/ImportCache.cpp
    Importer/CachingThread.cpp
    Importer/CopyThread.cpp
    Importer/LibraryImporter.cpp

    Threads/ReloadThread.cpp
    Threads/IndexDirectoriesThread.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_tagedit
    say_comp_playlist
    )

