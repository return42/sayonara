PROJECT(say_gui_info_dialog)

FILE(GLOB_RECURSE SOURCES . *.cpp)
FILE(GLOB_RECURSE RAW_UI_FILES . *.ui)

QT5_WRAP_UI(UI_FILES ${RAW_UI_FILES})

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_lyrics
	say_comp_metadata_info
	say_gui_tagedit
	say_gui_covers
)
