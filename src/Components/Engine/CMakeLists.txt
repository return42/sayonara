PROJECT(say_comp_engine)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

ADD_SUBDIRECTORY(Convert)
ADD_SUBDIRECTORY(Playback)
ADD_SUBDIRECTORY(Callbacks)

SET(SOURCES
    AbstractEngine.cpp
    AbstractPipeline.cpp
    EngineHandler.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${GSTREAMER_LIBRARIES}
	${GSTREAMER_BASE_LIBRARIES}
	${GSTREAMER_BASE_APP_LIBRARIES}
	say_comp_playmanager
	say_comp_engine_convert
	say_comp_engine_playback
)
