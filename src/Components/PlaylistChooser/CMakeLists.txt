PROJECT(say_comp_playlist_chooser)

SET(SOURCES
    PlaylistChooser.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_playlist
)
