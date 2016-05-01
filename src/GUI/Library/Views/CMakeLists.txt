PROJECT(say_gui_library_views)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(SOURCES
    LibraryGenreView.cpp
    LibraryViewAlbum.cpp
    LibraryView.cpp
    LibraryViewEvents.cpp
    LibraryViewHeader.cpp
    HeaderView.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_tagedit
	say_gui_library_helper
)
