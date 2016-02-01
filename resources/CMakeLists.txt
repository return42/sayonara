CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_resources)


FILE(WRITE sayonara "?package(sayonara): \\
  needs=\"X11\" \\
  section=\"Applications/Sound\" \\
  title=\"Sayonara\" \\
  command=\"${CMAKE_INSTALL_PREFIX}/bin/sayonara\" \\
  icon=\"${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps/sayonara.xpm\""
)


INSTALL(FILES "player.db" DESTINATION share/sayonara)
INSTALL(FILES "sayonara.desktop" DESTINATION share/applications)
#INSTALL(FILES "sayonara" DESTINATION share/menu)
INSTALL(FILES "sayonara.appdata.xml" DESTINATION share/appdata)
INSTALL(FILES "manpage/sayonara.1.gz" DESTINATION share/man/man1)
INSTALL(DIRECTORY "logos/" DESTINATION share/icons/hicolor)

