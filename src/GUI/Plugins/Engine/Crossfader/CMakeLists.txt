PROJECT(say_gui_plugin_crossfader)

SET(SOURCES
	GUI_Crossfader.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_Crossfader.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_plugins
)
