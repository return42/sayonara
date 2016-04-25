PROJECT(say_gui_global_message)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
        Message.cpp
	GlobalMessage.cpp
        GlobalMessageReceiverInterface.cpp

)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Qt5::Widgets
)

