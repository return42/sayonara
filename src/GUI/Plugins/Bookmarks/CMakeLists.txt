PROJECT(say_gui_plugin_bookmarks)

SET(SOURCES
	GUI_Bookmarks.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_Bookmarks.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_bookmarks
	say_interfaces_player_plugin
)
