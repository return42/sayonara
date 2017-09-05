PROJECT(say_gui_shutdown_dialog)

SET(SOURCES
	GUI_Shutdown.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_Shutdown.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_helper
    say_helper_shutdown
)
