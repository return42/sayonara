PROJECT(say_helper_settings)

SET(SOURCES
	Setting.cpp
	Settings.cpp
	SettingConverter.cpp
	SettingRegistry.cpp
	AbstrSettingNotifier.cpp
	SayonaraClass.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
	say_helper
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()
