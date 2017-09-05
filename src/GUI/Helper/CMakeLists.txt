PROJECT(say_gui_helper)

SET(SOURCES

	CustomMimeData.cpp
	GUI_Helper.cpp

	ContextMenu/ContextMenu.cpp
	ContextMenu/LibraryContextMenu.cpp

	Delegates/ComboBoxDelegate.cpp
	Delegates/StyledItemDelegate.cpp

	Dragable/Dragable.cpp

	IconLoader/IconLoader.cpp
	Library/LibraryDeleteDialog.cpp
	Library/GUI_EditLibrary.cpp

	MenuTool/MenuButton.cpp
	MenuTool/MenuTool.cpp

	RatingLabel/RatingLabel.cpp

	SayonaraWidget/SayonaraComboBox.cpp
	SayonaraWidget/SayonaraCompleter.cpp
	SayonaraWidget/SayonaraWidgetTemplate.cpp
	SayonaraWidget/SayonaraWidget.cpp
	SayonaraWidget/SayonaraDialog.cpp
	SayonaraWidget/SayonaraSlider.cpp
	SayonaraWidget/SayonaraLoadingBar.cpp

	SearchableWidget/SearchableView.cpp
	SearchableWidget/AbstractSearchModel.cpp
	SearchableWidget/SearchableFileTreeModel.cpp
	SearchableWidget/SayonaraSelectionView.cpp
	SearchableWidget/MiniSearcher.cpp

	Shortcuts/Shortcut.cpp
	Shortcuts/ShortcutHandler.cpp
	Shortcuts/ShortcutWidget.cpp

	Style/Style.cpp
)

QT5_WRAP_UI(UI_FILES
	Library/GUI_EditLibrary.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Widgets
	say_helper_settings
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

