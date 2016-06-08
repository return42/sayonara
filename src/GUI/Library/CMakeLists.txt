CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_library)

ADD_SUBDIRECTORY(InfoBox)
ADD_SUBDIRECTORY(ImportFolderDialog)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR},
	${CMAKE_CURRENT_BINARY_DIR}
)

SET(SOURCES
	GUI_AbstractLibrary.cpp
	GUI_LocalLibrary.cpp

	Models/LibraryItemModelAlbums.cpp
    Models/LibraryItemModelArtists.cpp
    Models/LibraryItemModel.cpp
    Models/LibraryItemModelTracks.cpp

    Delegates/LibraryItemDelegateAlbums.cpp
    Delegates/LibraryItemDelegateArtists.cpp
    Delegates/LibraryItemDelegateTracks.cpp
    Delegates/LibraryRatingDelegate.cpp

    Views/LibraryGenreView.cpp
    Views/LibraryViewAlbum.cpp
    Views/LibraryView.cpp
    Views/LibraryViewEvents.cpp
    Views/LibraryViewHeader.cpp
    Views/HeaderView.cpp

	Helper/DiscPopupMenu.cpp
	Helper/LocalLibraryMenu.cpp
	Helper/ColumnHeader.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_LocalLibrary.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_library
	say_comp_tagedit

	say_gui_helper
	say_gui_info_dialog

	say_gui_library_import_folder_dialog
	say_gui_library_info_box

	say_interfaces_library_container
)
