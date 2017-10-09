PROJECT(say_interfaces_preference_dialog)

SET(SOURCES
	PreferenceInterface.cpp
	PreferenceDialogInterface.cpp
	PreferenceWidgetInterface.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_utils
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

