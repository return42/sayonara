CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(ICONS
	bg-checker.png
	favicon.ico
	logo.png
)	

INSTALL(FILES "sayonara.png" DESTINATION /usr/share/icons)
INSTALL(FILES "sayonara.png" DESTINATION /usr/share/pixmaps)
INSTALL(FILES ${ICONS} DESTINATION share/sayonara)


