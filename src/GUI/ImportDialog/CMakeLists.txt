PROJECT(say_gui_import_dialog)

SET(SOURCES
	GUI_ImportDialog.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_ImportDialog.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_utils
    say_comp_library
	say_gui_tagedit
)
