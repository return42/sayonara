PROJECT(say_gui_library_info_box)

SET(SOURCES
    ./GUI_LibraryInfoBox.cpp
)

QT5_WRAP_UI(UI_HEADERS
    ./GUI_LibraryInfoBox.ui
)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_HEADERS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_helper
)
