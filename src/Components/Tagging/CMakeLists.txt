PROJECT(say_comp_tagging)

INCLUDE_DIRECTORIES(
    ${TAGLIB_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${TAGLIB_LIBRARY_DIRS})

SET(SOURCES
	ChangeNotifier.cpp  
	Editor.cpp  
	Expression.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
	say_utils
)

IF( WITH_COTIRE )
    cotire(${PROJECT_NAME})
ENDIF()


