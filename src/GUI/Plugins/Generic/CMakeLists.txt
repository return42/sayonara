PROJECT(say_gui_plugins)

SET(SOURCES
    GUI_PlayerPlugin.cpp
    PluginCloseButton.cpp
)

QT5_WRAP_UI(FORMS_UIC
    GUI_PlayerPlugin.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_interfaces_player_plugin
    say_gui_utils
)
