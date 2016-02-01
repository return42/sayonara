CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_plugin_audio_converter)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)


SET(SOURCES
	GUI_AudioConverter.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_AudioConvert.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_interfaces_player_plugin
    say_comp_engine
    say_gui_menu_tool
)
