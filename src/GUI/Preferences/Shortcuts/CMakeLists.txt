PROJECT( say_gui_prefs_shortcuts )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)


SET(SOURCES
    ./GUI_Shortcuts.cpp
	./GUI_ShortcutEntry.cpp
	./ShortcutLineEdit.cpp
)

QT5_WRAP_UI(UI_FILES
    ./GUI_Shortcuts.ui
	./GUI_ShortcutEntry.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		Qt5::Gui
		say_interfaces_preference_dialog
		say_gui_helper_shortcuts
		say_helper
)
