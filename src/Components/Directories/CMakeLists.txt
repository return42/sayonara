PROJECT(say_comp_directories)

SET(SOURCES
	FileOperations.cpp
	DirectoryReader.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
	say_utils
)
