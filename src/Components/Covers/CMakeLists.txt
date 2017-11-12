PROJECT(say_comp_covers)

SET(SOURCES
	AbstractCoverLookup.cpp
	CoverChangeNotifier.cpp
	CoverLookup.cpp
	CoverLookupAlternative.cpp
	CoverLocation.cpp
	CoverUtils.cpp

	GoogleCoverFetcher.cpp
	LFMCoverFetcher.cpp
	StandardCoverFetcher.cpp
	DiscogsCoverFetcher.cpp

	CoverFetchManager.cpp
	CoverFetchThread.cpp

	LocalCoverSearcher.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
	Qt5::Gui
)

IF( WITH_COTIRE )
	cotire(${PROJECT_NAME})
ENDIF()
