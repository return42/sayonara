PROJECT(say_comp_shutdown)

SET(SOURCES
	Shutdown.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_playmanager
	say_interfaces_notifications
	Qt5::DBus
)
