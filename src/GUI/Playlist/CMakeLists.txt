CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_playlist)

INCLUDE_DIRECTORIES(
		${CMAKE_CURRENT_BINARY_DIR},
		${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCES
		PlaylistMenu.cpp
		GUI_Playlist.cpp
		GUI_PlaylistTabs.cpp
		GUI_PlaylistEntryLook.cpp

		BottomBar/GUI_PlaylistBottomBar.cpp
		Delegate/PlaylistItemDelegate.cpp
		Model/PlaylistItemModel.cpp

		TabWidget/PlaylistTabBar.cpp
		TabWidget/PlaylistTabMenu.cpp
		TabWidget/PlaylistTabWidget.cpp
		View/PlaylistView.cpp
)

QT5_WRAP_UI(FORMS_UIC
		GUI_Playlist.ui
		GUI_PlaylistEntryLook.ui
		BottomBar/GUI_PlaylistBottomBar.ui
		View/GUI_PlaylistView.ui
)

SET(LINK_LIBS
	say_comp_playlist
	say_gui_info_dialog
	say_gui_shutdown_dialog
	say_gui_searchable_widget
	say_gui_context_menu
	say_gui_helper_shortcuts
	say_gui_global_message
	say_helper_directory_reader
)

IF(NOT ${WIN32})
	SET(LINK_LIBS
		${LINK_LIBS}
		say_helper_shutdown
	)
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		${LINK_LIBS}
)

