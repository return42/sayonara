PROJECT(say_comp_playlist)

SET(SOURCES
    AbstractPlaylist.cpp
    StdPlaylist.cpp
    StreamPlaylist.cpp
    PlaylistHandler.cpp
    PlaylistLoader.cpp
    PlaylistDBInterface.cpp
    PlaylistDBWrapper.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
    say_comp_playmanager
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

