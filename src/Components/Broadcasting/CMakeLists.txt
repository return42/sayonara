PROJECT(say_comp_broadcasting)

FILE(GLOB_RECURSE SOURCES . *.cpp)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_interfaces_raw_sound_receiver
	say_utils
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()
