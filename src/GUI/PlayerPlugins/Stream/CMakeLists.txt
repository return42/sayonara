CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_plugin_stream)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCES
	GUI_AbstractStream.cpp
	GUI_Podcasts.cpp
	GUI_Stream.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_Podcasts.ui
	GUI_Stream.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_streamplugins_streams
    say_interfaces_player_plugin
    say_gui_menu_tool
    say_gui_global_message
    say_gui_combobox_delegate
)
