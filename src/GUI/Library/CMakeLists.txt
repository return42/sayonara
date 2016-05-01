CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_library)

ADD_SUBDIRECTORY(Delegates)
ADD_SUBDIRECTORY(InfoBox)
ADD_SUBDIRECTORY(ImportFolderDialog)
ADD_SUBDIRECTORY(Models)
ADD_SUBDIRECTORY(Views)
ADD_SUBDIRECTORY(Helper)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR},
	${CMAKE_CURRENT_BINARY_DIR}
)

SET(SOURCES
	GUI_AbstractLibrary.cpp
	GUI_LocalLibrary.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_LocalLibrary.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_library

	say_gui_library_helper
	say_gui_info_dialog

	say_gui_library_delegates
	say_gui_library_import_folder_dialog
	say_gui_library_info_box
	say_gui_library_models
	say_gui_library_views

	say_interfaces_library_container

)
