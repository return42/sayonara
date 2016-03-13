CMAKE_MINIMUM_REQUIRED(VERSION 2.7)
PROJECT(say_application)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCES
    application.cpp
    InstanceThread.cpp
)

SET(APPLICATION_LIBRARIES
    say_gui_library
    say_gui_directory_widget

    say_gui_plugin_audio_converter
    say_gui_plugin_bookmarks
    say_gui_plugin_broadcasting
    say_gui_plugin_engine
    say_gui_plugin_equalizer
    say_gui_plugin_playlist_chooser
    say_gui_plugin_speed
    say_gui_plugin_stream

    say_gui_notifications
    say_gui_lastfm
    say_gui_stream_recorder
    say_gui_startup_dialog
    say_gui_language_chooser
    say_gui_player
    say_gui_fontconfig
    say_gui_remote_control
    say_gui_library_preferences

    say_interfaces_libraryinterface
    say_comp_remote_control
)

IF(NOT ${WIN32})
    SET(APPLICATION_LIBRARIES
	${APPLICATION_LIBRARIES}
	say_comp_dbus
    )
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${APPLICATION_LIBRARIES}
)


