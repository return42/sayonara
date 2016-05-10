PROJECT( sayonara_somafm )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(SOURCES
    ./GUI_SomaFM.cpp
    SomaFMLibrary.cpp
    SomaFMLinkListModel.cpp
)

QT5_WRAP_UI(UI_FILES
    ./GUI_SomaFM.ui
)

QT5_ADD_RESOURCES(RESOURCES
    SomaFMIcons.qrc
)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCES} ${UI_FILES} ${RESOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Widgets
	Qt5::Core
	say_gui_library
	say_interfaces_library_container
)


INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib${LIB_SUFFIX}/sayonara)
