PROJECT( say_gui_preferences )

FILE(GLOB_RECURSE SOURCES . *.cpp)
FILE(GLOB_RECURSE RAW_UI_FILES . *.ui)

QT5_WRAP_UI(UI_FILES ${RAW_UI_FILES})

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_preferences
	say_comp_streamplugins
	say_comp_covers
	say_comp_library
	say_interfaces_preference_dialog
)


IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()

