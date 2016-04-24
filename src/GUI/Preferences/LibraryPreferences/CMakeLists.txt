PROJECT( say_gui_prefs_library )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(SOURCES
    ./GUI_LibraryPreferences.cpp
)


QT5_WRAP_UI(FORMS_UIC
	GUI_LibraryPreferences.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_interfaces_preference_dialog
)
