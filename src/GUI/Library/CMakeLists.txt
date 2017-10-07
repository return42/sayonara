PROJECT(say_gui_library)

SET(SOURCES
    GUI_AbstractLibrary.cpp
    GUI_LocalLibrary.cpp
	GUI_EmptyLibrary.cpp
    LocalLibraryContainer.cpp
	EmptyLibraryContainer.cpp

	Models/AlbumModel.cpp
	Models/ArtistModel.cpp
	Models/ItemModel.cpp
	Models/TrackModel.cpp
	Models/CoverModel.cpp
    Models/AlbumCoverFetchThread.cpp

    Delegates/RatingDelegate.cpp
	Delegates/CoverDelegate.cpp

    Views/GenreView.cpp
	Views/AlbumView.cpp
    Views/View.cpp
    Views/TableView.cpp
    Views/HeaderView.cpp
    Views/CoverView.cpp

    Helper/DiscPopupMenu.cpp
    Helper/LocalLibraryMenu.cpp
    Helper/ColumnHeader.cpp

    InfoBox/GUI_LibraryInfoBox.cpp
    ImportFolderDialog/GUI_ImportFolder.cpp
)

QT5_WRAP_UI(FORMS_UIC
    GUI_LocalLibrary.ui
	GUI_EmptyLibrary.ui
    InfoBox/GUI_LibraryInfoBox.ui
    ImportFolderDialog/GUI_ImportFolder.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_library
    say_comp_tagedit

    say_gui_helper
    say_gui_info_dialog

    say_interfaces_library_container
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

