PROJECT(say_gui_searchable_widget)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
	SearchableListView.cpp
	SearchableTableView.cpp
	SearchableFileTreeView.cpp
	SayonaraSelectionView.cpp
	MiniSearcher.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_combobox_delegate
)
