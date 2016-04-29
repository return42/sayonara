CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_plugin_playlist_chooser)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_SUBDIRECTORY(TargetPlaylistDialog)

SET(SOURCES
    GUI_PlaylistChooser.cpp
)

QT5_WRAP_UI(FORMS_UIC
    GUI_PlaylistChooser.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_target_playlist_dialog
    say_comp_playlist_chooser
    say_interfaces_player_plugin
)
