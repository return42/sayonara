CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_info_dialog)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCES
    GUI_InfoDialog.cpp
)

QT5_WRAP_UI(FORMS_UIC
    GUI_InfoDialog.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_lyrics
    say_helper_metadata_info
    say_gui_tagedit
    say_gui_alternative_covers
)


