PROJECT(say_interfaces_raw_sound_receiver)

SET(SOURCES
	RawSoundReceiverInterface.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Qt5::Core
)
