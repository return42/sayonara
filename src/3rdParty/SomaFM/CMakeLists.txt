PROJECT( sayonara_somafm )

SET(SOURCES
    SomaFMLibrary.cpp
    SomaFMStation.cpp

    ui/GUI_SomaFM.cpp
    ui/SomaFMLibraryContainer.cpp
    ui/SomaFMStationModel.cpp
    ui/SomaFMPlaylistModel.cpp
    ui/SomaFMStationView.cpp
)

QT5_WRAP_UI(UI_FILES
    ui/GUI_SomaFM.ui
)

QT5_ADD_RESOURCES(RESOURCES
    ui/SomaFMIcons.qrc
)

IF( WIN32 )
    ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES} ${RESOURCES})
ELSE()
    ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCES} ${UI_FILES} ${RESOURCES})
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_utils
    say_comp_playlist
    say_interfaces_library_container
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()

IF(NOT WIN32 OR NOT DEFINED WIN32)
    INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib${LIB_SUFFIX}/sayonara)
ENDIF()
