PROJECT(say_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

ADD_SUBDIRECTORY(MetaDataInfo)
ADD_SUBDIRECTORY(Settings)
ADD_SUBDIRECTORY(Compressor)

IF(NOT WIN32 OR NOT DEFINED WIN32)
    ADD_SUBDIRECTORY(Shutdown)
ENDIF()


SET(SOURCES

    EqualizerPresets.cpp
    FileHelper.cpp
    Helper.cpp
    Language.cpp
    UrlHelper.cpp
    Set.cpp
    Tree.cpp

    DirectoryReader/DirectoryReader.cpp


    Library/SearchMode.cpp
    Logger/Logger.cpp

    Message/Message.cpp
    Message/GlobalMessage.cpp
    Message/GlobalMessageReceiverInterface.cpp

    MetaData/LibraryItem.cpp
    MetaData/MetaData.cpp
    MetaData/MetaDataList.cpp
    MetaData/Album.cpp
    MetaData/Artist.cpp

    Parser/AbstractPlaylistParser.cpp
    Parser/M3UParser.cpp
    Parser/PLSParser.cpp
    Parser/ASXParser.cpp
    Parser/PlaylistParser.cpp
    Parser/PodcastParser.cpp
    Parser/StreamParser.cpp
    Parser/CommandLineParser.cpp

    Playlist/CustomPlaylist.cpp
    Playlist/CustomPlaylistSkeleton.cpp
    Playlist/PlaylistMode.cpp

    Random/RandomGenerator.cpp
    #SmartCompare/SmartCompare.cpp

    Tagging/Tagging.cpp

    Tagging/Frames/AbstractFrame.cpp
    Tagging/Frames/Discnumber.cpp
    Tagging/Frames/Popularimeter.cpp
    Tagging/Frames/Cover.cpp

    WebAccess/AsyncWebAccess.cpp
)

#ADD_DEFINITIONS( -DSMART_COMPARE" )


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Qt5::Network
    Qt5::Xml
    Qt5::DBus
    Qt5::Gui
    tag
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

