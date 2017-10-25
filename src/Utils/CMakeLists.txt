PROJECT(say_utils)

ADD_SUBDIRECTORY(MetaDataInfo)
ADD_SUBDIRECTORY(Compressor)

IF(NOT WIN32 OR NOT DEFINED WIN32)
	ADD_SUBDIRECTORY(Shutdown)
ENDIF()

INCLUDE(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("cxxabi.h" HAVE_CXX_ABI)
IF(${HAVE_CXX_ABI})
	ADD_DEFINITIONS("-DHAVE_CXX_ABI")
	MESSAGE("Compile with demangle")
ELSE()
	MESSAGE("Demangle not found")
ENDIF()

SET(SOURCES
    EqualizerPresets.cpp
    FileUtils.cpp
    Language.cpp
    Utils.cpp
    Set.cpp
    Tree.cpp

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

	Settings/SayonaraClass.cpp
	Settings/Setting.cpp
	Settings/Settings.cpp
	Settings/SettingConverter.cpp
	Settings/SettingNotifier.cpp
	Settings/SettingRegistry.cpp

    SimilarArtists/SimilarArtists.cpp

	StreamRecorder/StreamRecorderUtils.cpp

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
#	Tagging/MappedFilestream;

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
