PROJECT( say_gui_preferences )

SET(SOURCES
	Broadcast/GUI_BroadcastSetup.cpp
	Covers/GUI_Covers.cpp
	Fonts/GUI_FontConfig.cpp
	Icons/GUI_IconPreferences.cpp
	Language/GUI_LanguageChooser.cpp
	LastFM/GUI_LastFM.cpp
	LibraryPreferences/GUI_LibraryPreferences.cpp
	LibraryPreferences/LibraryListModel.cpp
	LibraryPreferences/ChangeOperations.cpp
	Notifications/GUI_Notifications.cpp
	PlayerPreferences/GUI_PlayerPreferences.cpp
	PlaylistPreferences/GUI_PlaylistPreferences.cpp
	PreferenceDialog/GUI_PreferenceDialog.cpp
	ProxyPreferences/GUI_Proxy.cpp
	RemoteControl/GUI_RemoteControl.cpp
	#Search/GUI_SearchPreferences.cpp
	Shortcuts/GUI_Shortcuts.cpp
	Shortcuts/GUI_ShortcutEntry.cpp
	Shortcuts/ShortcutLineEdit.cpp
	StreamRecorder/GUI_StreamRecorder.cpp
	Streams/GUI_StreamPreferences.cpp
)

QT5_WRAP_UI(UI_FILES
	Broadcast/GUI_BroadcastSetup.ui
	Covers/GUI_Covers.ui
	Fonts/GUI_FontConfig.ui
	Icons/GUI_IconPreferences.ui
	Language/GUI_LanguageChooser.ui
	LastFM/GUI_LastFM.ui
	LibraryPreferences/GUI_LibraryPreferences.ui
	Notifications/GUI_Notifications.ui
	PlayerPreferences/GUI_PlayerPreferences.ui
	PlaylistPreferences/GUI_PlaylistPreferences.ui
	PreferenceDialog/GUI_PreferenceDialog.ui
	ProxyPreferences/GUI_Proxy.ui
	RemoteControl/GUI_RemoteControl.ui
	Shortcuts/GUI_Shortcuts.ui
	Shortcuts/GUI_ShortcutEntry.ui
	#Search/GUI_SearchPreferences.ui
	StreamRecorder/GUI_StreamRecorder.ui
	Streams/GUI_StreamPreferences.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_preferences
	say_comp_streamplugins_lastfm
	say_comp_covers
	say_comp_library
	say_interfaces_preference_dialog
)


IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()

