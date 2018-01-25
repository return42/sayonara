PROJECT(say_comp_library)

FILE(GLOB_RECURSE SOURCES . *.cpp)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
	say_comp_tagging
	say_comp_playlist
	say_comp_directories
)

