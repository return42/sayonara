PROJECT( say_gui_library_helper )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(SOURCES
    ./DiscPopupMenu.cpp
    ./LocalLibraryMenu.cpp
    ./ColumnHeader.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_helper
)
