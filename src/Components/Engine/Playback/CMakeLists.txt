PROJECT(say_comp_engine_playback)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(SOURCES
	PipelineProbes.cpp
	PlaybackEngine.cpp
	PlaybackPipeline.cpp
	StreamRecorder.cpp

)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${GSTREAMER_LIBRARIES}
	${GSTREAMER_BASE_LIBRARIES}
	${GSTREAMER_BASE_APP_LIBRARIES}
	say_comp_engine_callbacks
	say_helper_tagging
	say_helper_parser
)
