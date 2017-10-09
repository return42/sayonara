PROJECT( say_utils_metadata_info )

SET(SOURCES
    ./AlbumInfo.cpp
    ./MetaDataInfo.cpp
    ./ArtistInfo.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_utils
	say_comp_covers
	say_utils_compressor
)
