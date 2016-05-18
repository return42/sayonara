PROJECT(say_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

ADD_SUBDIRECTORY(MetaDataInfo)
ADD_SUBDIRECTORY(Parser)
ADD_SUBDIRECTORY(Settings)
ADD_SUBDIRECTORY(Tagging)
ADD_SUBDIRECTORY(Playlist)
ADD_SUBDIRECTORY(Compressor)
ADD_SUBDIRECTORY(DirectoryReader)

IF(NOT ${WIN32})
    ADD_SUBDIRECTORY(Shutdown)
ENDIF()


SET(SOURCES

    EqualizerPresets.cpp
    FileHelper.cpp
    Helper.cpp
    UrlHelper.cpp
    Set.cpp
    Tree.cpp

    MetaData/LibraryItem.cpp
    MetaData/MetaData.cpp
    MetaData/MetaDataList.cpp
    MetaData/Album.cpp
    MetaData/Artist.cpp

    Logger/Logger.cpp
    Random/RandomGenerator.cpp
    SmartCompare/SmartCompare.cpp
    WebAccess/AsyncWebAccess.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Qt5::Network
    Qt5::Xml
    Qt5::DBus
    Qt5::Gui

    ${HELPER_LIBRARIES}
)
