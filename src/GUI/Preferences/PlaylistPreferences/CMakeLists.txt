PROJECT( say_gui_prefs_playlist )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(SOURCES
    ./GUI_PlaylistPreferences.cpp
)

QT5_WRAP_UI(UI_FILES
    ./GUI_PlaylistPreferences.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
#edit me
)
