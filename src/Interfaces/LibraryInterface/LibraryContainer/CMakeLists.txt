PROJECT(say_interfaces_library_container)

SET(SOURCES
	LibraryContainer.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Core
	Qt5::Widgets
)
