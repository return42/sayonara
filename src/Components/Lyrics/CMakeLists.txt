PROJECT(say_comp_lyrics)

SET(SOURCES
        Lyrics.cpp
	LyricLookup.cpp
	LyricServer.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_utils
)
