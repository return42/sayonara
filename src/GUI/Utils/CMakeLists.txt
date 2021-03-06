PROJECT(say_gui_utils)

SET(SOURCES

	CustomMimeData.cpp
	MimeDataUtils.cpp
	GuiUtils.cpp
	EventFilter.cpp
	Icons.cpp
	RatingLabel.cpp
	Style.cpp

	ContextMenu/ContextMenu.cpp
	ContextMenu/LibraryContextMenu.cpp

	Delegates/ComboBoxDelegate.cpp
	Delegates/StyledItemDelegate.cpp

	InputDialog/LineInputDialog.cpp

	Library/GUI_EditLibrary.cpp
	Library/GUI_DeleteDialog.cpp

	MenuTool/MenuButton.cpp
	MenuTool/MenuTool.cpp

	Widgets/ComboBox.cpp
	Widgets/Completer.cpp
	Widgets/Dragable.cpp
	Widgets/WidgetTemplate.cpp
	Widgets/Widget.cpp
	Widgets/Dialog.cpp
	Widgets/Slider.cpp
	Widgets/ProgressBar.cpp

	SearchableWidget/SearchableView.cpp
	SearchableWidget/SearchableModel.cpp
	SearchableWidget/SelectionView.cpp
	SearchableWidget/MiniSearcher.cpp

	Shortcuts/Shortcut.cpp
	Shortcuts/ShortcutHandler.cpp
	Shortcuts/ShortcutWidget.cpp

	PreferenceAction.cpp
)

QT5_WRAP_UI(UI_FILES
	Library/GUI_EditLibrary.ui
	Library/GUI_DeleteDialog.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Widgets
	Qt5::DBus
	say_utils
	say_comp_directories
	say_comp_preferences
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()

