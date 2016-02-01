PROJECT(say_gui_context_menu)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
	ContextMenu.cpp
	LibraryContextMenu.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
	
