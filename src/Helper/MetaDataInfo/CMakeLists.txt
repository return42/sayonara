PROJECT( say_helper_metadata_info )

SET(SOURCES
    ./AlbumInfo.cpp
    ./MetaDataInfo.cpp
    ./ArtistInfo.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_helper
	say_comp_covers
	say_helper_compressor
)
