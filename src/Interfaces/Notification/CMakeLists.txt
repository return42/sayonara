PROJECT(say_interfaces_notifications)

SET(SOURCES
	NotificationInterface.cpp
	NotificationHandler.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_utils
)


IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

