PROJECT(say_gui_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


ADD_SUBDIRECTORY(ComboBoxDelegate)
ADD_SUBDIRECTORY(ContextMenu)
ADD_SUBDIRECTORY(GlobalMessage)
ADD_SUBDIRECTORY(MenuTool)
ADD_SUBDIRECTORY(RatingLabel)
ADD_SUBDIRECTORY(SearchableWidget)
ADD_SUBDIRECTORY(Delegates)
ADD_SUBDIRECTORY(Shortcuts)

SET(SOURCES
	SayonaraWidgetTemplate.cpp
	SayonaraWidget.cpp
	SayonaraDialog.cpp
	SayonaraShortcutWidget.cpp
	IconLoader/IconLoader.cpp
	CustomMimeData.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Core
	Qt5::Widgets
	say_helper
)
