CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(sayonara_soundcloud)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ui)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(SOURCES
	src/SoundcloudData.cpp
	src/SoundcloudWebAccess.cpp
	src/SoundcloudJsonParser.cpp
	src/SoundcloudLibrary.cpp
	src/SoundcloudDataFetcher.cpp
	ui/GUI_SoundcloudLibrary.cpp
	ui/GUI_SoundcloudArtistSearch.cpp
)

SET(FORMS
	ui/GUI_SoundcloudLibrary.ui
	ui/GUI_SoundcloudArtistSearch.ui
)

SET(RESOURCES
	SoundcloudIcons.qrc
)

QT5_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

QT5_WRAP_UI(FORMS_HEADERS ${FORMS})

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCES} ${FORMS_HEADERS} ${RESOURCES_RCC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
	say_helper
	say_gui_helper
	say_gui_library
	say_interfaces_library_container
)


INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib${LIB_SUFFIX}/sayonara)
INSTALL(FILES "soundcloud.db" DESTINATION share/sayonara)

