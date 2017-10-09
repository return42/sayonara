PROJECT(say_comp_directory_reader)
SET(SOURCES
	DirectoryReader.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
	say_utils
)
