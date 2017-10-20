PROJECT(say_comp_engine)

SET(SOURCES
    AbstractEngine.cpp
    AbstractPipeline.cpp
    EngineHandler.cpp

	Playback/EqualizerHandler.cpp
    Playback/PipelineProbes.cpp
    Playback/PlaybackEngine.cpp
    Playback/PlaybackPipeline.cpp

    Playback/Crossfader.cpp
    Playback/ChangeablePipeline.cpp
	Playback/SeekHandler.cpp
    Playback/SoundOutReceiver.cpp
	Playback/SpeedHandler.cpp

	Playback/StreamRecorder.cpp
    Playback/StreamRecorderData.cpp
	Playback/StreamRecorderHandler.cpp
	Playback/StreamRecorderUtils.cpp

    Convert/ConvertEngine.cpp
    Convert/ConvertPipeline.cpp

    Callbacks/EngineCallbacks.cpp
    Callbacks/PipelineCallbacks.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${GSTREAMER_LIBRARIES}
    ${GSTREAMER_BASE_LIBRARIES}
    ${GSTREAMER_BASE_APP_LIBRARIES}

	say_comp_playmanager
	say_utils
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()
