PROJECT( say_gui_player_preferences )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(SOURCES
    ./GUI_PlayerPreferences.cpp
)

QT5_WRAP_UI(UI_FILES
    ./GUI_PlayerPreferences.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_interfaces_preference_dialog
)
