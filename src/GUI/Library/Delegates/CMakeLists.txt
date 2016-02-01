PROJECT(say_gui_library_delegates)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
    ./LibraryItemDelegateAlbums.cpp
    ./LibraryItemDelegateArtists.cpp
    ./LibraryItemDelegateTracks.cpp
    ./LibraryRatingDelegate.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_rating_label
)
