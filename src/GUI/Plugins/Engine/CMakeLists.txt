PROJECT(say_gui_plugin_engine)

SET(SOURCES
	EngineColorStyleChooser.cpp
	EnginePlugin.cpp
	GUI_LevelPainter.cpp
	GUI_Spectrum.cpp
	GUI_StyleSettings.cpp

	AudioConverter/GUI_AudioConverter.cpp
	Crossfader/GUI_Crossfader.cpp
	Speed/GUI_Speed.cpp
	Equalizer/EqSlider.cpp
	Equalizer/GUI_Equalizer.cpp
	Speed/GUI_Speed.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_LevelPainter.ui
	GUI_Spectrum.ui
	GUI_Style.ui

	AudioConverter/GUI_AudioConverter.ui
	Crossfader/GUI_Crossfader.ui
	Equalizer/GUI_Equalizer.ui
	Speed/GUI_Speed.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_engine
	say_interfaces_player_plugin
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()
