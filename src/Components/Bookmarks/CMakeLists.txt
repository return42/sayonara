PROJECT(say_comp_bookmarks)

SET(SOURCES
    Bookmarks.cpp
    Bookmark.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_playmanager
)
