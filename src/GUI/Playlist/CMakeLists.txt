PROJECT(say_gui_playlist)

SET(SOURCES
	GUI_Playlist.cpp
	GUI_PlaylistTabs.cpp
	BookmarksMenu.cpp

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
	BottomBar/GUI_PlaylistBottomBar.ui
	View/GUI_PlaylistView.ui
)

SET(LINK_LIBS
	say_comp_playlist
	say_gui_info_dialog
	say_gui_shutdown_dialog
	say_gui_utils
	say_comp_bookmarks
)

IF(NOT WIN32 OR NOT DEFINED WIN32)
SET(LINK_LIBS
	${LINK_LIBS}
	say_utils_shutdown
)
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${LINK_LIBS}
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()


