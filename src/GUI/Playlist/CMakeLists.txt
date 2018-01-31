PROJECT(say_gui_playlist)

FILE(GLOB_RECURSE SOURCES . *.cpp)
FILE(GLOB_RECURSE RAW_UI_FILES . *.ui)

QT5_WRAP_UI(UI_FILES ${RAW_UI_FILES})

SET(LINK_LIBS
	say_comp_bookmarks
	say_comp_playlist
	say_comp_preferences
	say_gui_info_dialog
	say_gui_shutdown_dialog
	say_gui_utils
)

IF(NOT WIN32 OR NOT DEFINED WIN32)
SET(LINK_LIBS
	${LINK_LIBS}
	say_comp_shutdown
)
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${LINK_LIBS}
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()


