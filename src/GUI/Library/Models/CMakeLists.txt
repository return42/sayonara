PROJECT(say_gui_library_models)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES

    ./LibraryItemModelAlbums.cpp
    ./LibraryItemModelArtists.cpp
    ./LibraryItemModel.cpp
    ./LibraryItemModelTracks.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
