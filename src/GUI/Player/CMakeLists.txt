PROJECT(say_gui_player)

FILE(GLOB_RECURSE SOURCES . *.cpp)
FILE(GLOB_RECURSE RAW_UI_FILES . *.ui)

QT5_WRAP_UI(UI_FILES ${RAW_UI_FILES})

SET(PLAYER_LIBRARIES

	say_gui_playlist
	say_interfaces_player_plugin
	say_interfaces_notifications
	say_interfaces_preference_dialog
	say_gui_plugins
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${PLAYER_LIBRARIES}
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()

