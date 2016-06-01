PROJECT( say_gui_preferences )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_SUBDIRECTORY(Broadcast)
ADD_SUBDIRECTORY(Fonts)
ADD_SUBDIRECTORY(Language)
ADD_SUBDIRECTORY(LastFM)
ADD_SUBDIRECTORY(LibraryPreferences)
ADD_SUBDIRECTORY(Notifications)
ADD_SUBDIRECTORY(PlayerPreferences)
ADD_SUBDIRECTORY(PlaylistPreferences)
ADD_SUBDIRECTORY(PreferenceDialog)
ADD_SUBDIRECTORY(RemoteControl)
ADD_SUBDIRECTORY(Shortcuts)
ADD_SUBDIRECTORY(StreamRecorder)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_prefs_broadcast
	say_gui_prefs_fonts
	say_gui_prefs_language
	say_gui_prefs_lastfm
	say_gui_prefs_library
	say_gui_prefs_notifications
	say_gui_prefs_player
	say_gui_prefs_playlist
	say_gui_preference_dialog
	say_gui_prefs_remote_control
	say_gui_prefs_shortcuts
	say_gui_prefs_stream_recorder
)
