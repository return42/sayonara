CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(say_gui_plugins)

#ADD_SUBDIRECTORY(Generic)
#ADD_SUBDIRECTORY(Bookmarks)
#ADD_SUBDIRECTORY(Broadcasting)
#ADD_SUBDIRECTORY(Engine)
#ADD_SUBDIRECTORY(PlaylistChooser)
#ADD_SUBDIRECTORY(Stream)


SET(SOURCES
	Bookmarks/GUI_Bookmarks.cpp
	Broadcasting/GUI_Broadcast.cpp

	Engine/EngineColorStyleChooser.cpp
	Engine/EnginePlugin.cpp
	Engine/GUI_LevelPainter.cpp
	Engine/GUI_Spectrum.cpp
	Engine/GUI_StyleSettings.cpp

	Engine/AudioConverter/GUI_AudioConverter.cpp
	Engine/Crossfader/GUI_Crossfader.cpp
	Engine/Speed/GUI_Speed.cpp
	Engine/Equalizer/EqSlider.cpp
	Engine/Equalizer/GUI_Equalizer.cpp
	Engine/Speed/GUI_Speed.cpp

	Generic/GUI_PlayerPlugin.cpp
	Generic/PluginCloseButton.cpp

	PlaylistChooser/GUI_PlaylistChooser.cpp
	PlaylistChooser/GUI_TargetPlaylistDialog.cpp

	Stream/GUI_AbstractStream.cpp
	Stream/GUI_Podcasts.cpp
	Stream/GUI_Stream.cpp


)

QT5_WRAP_UI(FORMS_UIC
	Bookmarks/GUI_Bookmarks.ui
	Broadcasting/GUI_Broadcast.ui

	Engine/GUI_LevelPainter.ui
	Engine/GUI_Spectrum.ui
	Engine/GUI_Style.ui

	Engine/AudioConverter/GUI_AudioConverter.ui
	Engine/Crossfader/GUI_Crossfader.ui
	Engine/Equalizer/GUI_Equalizer.ui
	Engine/Speed/GUI_Speed.ui

	Generic/GUI_PlayerPlugin.ui

	PlaylistChooser/GUI_PlaylistChooser.ui
	PlaylistChooser/GUI_TargetPlaylistDialog.ui

	Stream/GUI_Podcasts.ui
	Stream/GUI_Stream.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_bookmarks
	say_comp_broadcasting
	say_comp_engine
	say_comp_playlist_chooser
	say_comp_streamplugins_streams

	say_interfaces_player_plugin
)
