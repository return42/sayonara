PROJECT(say_interfaces_libraryinterface)

ADD_SUBDIRECTORY(LibraryContainer)

SET(SOURCES
    LibraryPluginHandler.cpp
	LibraryPluginCombobox.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_utils
    say_gui_library
)


