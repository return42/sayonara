PROJECT(say_helper_settings)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
	Setting.cpp
	Settings.cpp
	SettingConverter.cpp
	AbstrSettingNotifier.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Core
)
