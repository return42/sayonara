PROJECT(say_db)

SET(HEADERS
    CDatabaseConnector.h
)

SET(SOURCES

    AbstractDatabase.cpp
	LocalLibraryDatabase.cpp
    LibraryDatabase.cpp
    DatabaseSearchMode.cpp
    DatabaseBookmarks.cpp
    DatabaseConnector.cpp
    DatabaseModule.cpp
    DatabaseAlbums.cpp
    DatabaseArtists.cpp
    DatabaseLibrary.cpp
    DatabasePlaylist.cpp
    DatabasePodcasts.cpp
    DatabaseSettings.cpp
    DatabaseStreams.cpp
    DatabaseTracks.cpp
    DatabaseVisStyles.cpp

    SayonaraQuery.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Sql
	say_utils
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()


