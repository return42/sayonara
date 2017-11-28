PROJECT(say_gui_directory_widget)

SET(SOURCES
	DirectoryContextMenu.cpp
	GUI_DirectoryWidget.cpp
	DirectoryIconProvider.cpp
	DirectoryDelegate.cpp
	DirectoryModel.cpp
	FileListView.cpp
	FileListModel.cpp
	DirectoryTreeView.cpp
	DirectoryWidgetContainer.cpp
)

QT5_WRAP_UI(UI_HEADERS
	GUI_DirectoryWidget.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_HEADERS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_info_dialog
	say_gui_import_dialog
	say_comp_directories
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()



