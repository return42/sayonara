PROJECT(say_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

ADD_SUBDIRECTORY(MetaData)
ADD_SUBDIRECTORY(Parser)
ADD_SUBDIRECTORY(Settings)
ADD_SUBDIRECTORY(Tagging)
ADD_SUBDIRECTORY(Playlist)
ADD_SUBDIRECTORY(Compressor)
ADD_SUBDIRECTORY(DirectoryReader)

IF(NOT ${WIN32})
    ADD_SUBDIRECTORY(Shutdown)
ENDIF()


SET(HEADERS

    MetaData/MetaDataInfo.h
    Shutdown/Shutdown.h
    WebAccess/AsyncWebAccess.h
    SayonaraClass.h
    Settings/AbstrSettingNotifier.h
)

SET(SOURCES

    Helper.cpp
    FileHelper.cpp
    UrlHelper.cpp
    Set.cpp

    DirectoryReader/DirectoryReader.cpp
    Logger/Logger.cpp
    Random/RandomGenerator.cpp
    SmartCompare/SmartCompare.cpp
    Style/Style.cpp
    SayonaraClass.cpp
    Tree/Tree.cpp
    WebAccess/AsyncWebAccess.cpp
)

SET(HELPER_LIBRARIES

    say_comp_covers
    say_db
    say_gui_global_message
	say_helper_metadata
    say_helper_settings
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Qt5::Network
    Qt5::Xml
    Qt5::DBus

    ${HELPER_LIBRARIES}
)
