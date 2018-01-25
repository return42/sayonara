PROJECT(say_gui_library)

FILE(GLOB_RECURSE SOURCES . *.cpp)
FILE(GLOB_RECURSE RAW_UI_FILES . *.ui)

QT5_WRAP_UI(UI_FILES ${RAW_UI_FILES})

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_library
	say_comp_tagging

	say_gui_utils
	say_gui_info_dialog
	say_gui_import_dialog

	say_interfaces_library_container
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()

