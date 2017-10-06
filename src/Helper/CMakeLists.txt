PROJECT(say_helper)

ADD_SUBDIRECTORY(MetaDataInfo)
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

    Library/Filter.cpp
    Library/SearchMode.cpp
    Library/Sorting.cpp
	Library/LibraryInfo.cpp

    Logger/Logger.cpp

    Message/Message.cpp
    Message/GlobalMessage.cpp
    Message/GlobalMessageReceiverInterface.cpp

    MetaData/LibraryItem.cpp
    MetaData/MetaData.cpp
    MetaData/MetaDataList.cpp
    MetaData/Album.cpp
    MetaData/Artist.cpp
    MetaData/Genre.cpp

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

	Settings/Setting.cpp
	Settings/Settings.cpp
	Settings/SettingConverter.cpp
	Settings/SettingRegistry.cpp
	Settings/SayonaraClass.cpp

    SimilarArtists/SimilarArtists.cpp
    #SmartCompare/SmartCompare.cpp

    Tagging/Tagging.cpp

    Tagging/AbstractFrame.cpp
    Tagging/ID3v2/Discnumber.cpp
    Tagging/ID3v2/Popularimeter.cpp
    Tagging/ID3v2/Cover.cpp
    Tagging/ID3v2/AlbumArtist.cpp
    Tagging/ID3v2/Lyrics.cpp
    Tagging/MP4/Cover.cpp
    Tagging/MP4/DiscnumberFrame.cpp
    Tagging/MP4/AlbumArtist.cpp
    Tagging/MP4/PopularimeterFrame.cpp
    Tagging/Xiph/AlbumArtist.cpp
    Tagging/Xiph/DiscnumberFrame.cpp
    Tagging/Xiph/PopularimeterFrame.cpp
    Tagging/Xiph/LyricsFrame.cpp

    Tagging/Models/Discnumber.cpp
    Tagging/Models/Cover.cpp
    Tagging/Models/Popularimeter.cpp

    WebAccess/AsyncWebAccess.cpp
    WebAccess/IcyWebAccess.cpp
	WebAccess/Proxy.cpp
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
