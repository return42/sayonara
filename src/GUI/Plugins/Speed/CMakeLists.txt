CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_plugin_speed)

SET(SOURCES
	GUI_Speed.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_Speed.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_interfaces_player_plugin
    say_comp_engine
)
