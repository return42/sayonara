PROJECT(say_interfaces_player_plugin)

SET(SOURCES
	PlayerPlugin.cpp
	PlayerPluginHandler.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_utils
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

