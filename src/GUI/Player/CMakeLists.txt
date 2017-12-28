PROJECT(say_gui_player)

SET(SOURCES
	GUI_Controls.cpp
	GUI_Player.cpp
	GUI_PlayerMenubar.cpp
	GUI_PlayerMessages.cpp
	GUI_TrayIcon.cpp
	GUI_Logger.cpp
	SearchSlider.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_Controls.ui
	GUI_Player.ui
	GUI_Logger.ui
)

SET(PLAYER_LIBRARIES

	say_gui_playlist
	say_interfaces_player_plugin
	say_interfaces_notifications
	say_interfaces_preference_dialog
	say_gui_plugins
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${PLAYER_LIBRARIES}
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()

