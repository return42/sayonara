PROJECT(say_comp_dbus)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)


SET(HEADERS
	DBusMPRIS.h
	DBusMediaKeysInterface.h
	DBusMediaKeysInterfaceMate.h
	DBusMediaKeysInterfaceGnome.h
)

SET(SOURCES
	DBusHandler.cpp
	DBusMPRIS.cpp
	DBusMediaKeysInterface.cpp
	DBusMediaKeysInterfaceMate.cpp
	DBusMediaKeysInterfaceGnome.cpp
	DBusNotifications.cpp
)

QT5_ADD_DBUS_ADAPTOR(SOURCES
    resources/org.mpris.MediaPlayer2.xml
    DBusMPRIS.h
    DBusMPRIS::MediaPlayer2
    org_mpris_media_player2_adaptor
    OrgMprisMediaPlayer2Adaptor
)

QT5_ADD_DBUS_ADAPTOR(SOURCES
    resources/org.mpris.MediaPlayer2.Player.xml
    DBusMPRIS.h
    DBusMPRIS::MediaPlayer2
    org_mpris_media_player2_player_adaptor
    OrgMprisMediaPlayer2PlayerAdaptor
)

QT5_ADD_DBUS_INTERFACE(SOURCES
    resources/org.freedesktop.Notifications.xml
    Notifications
)

#QT5_ADD_DBUS_INTERFACE(SOURCES
#	resources/org.libmtp.mtpd.device.xml
#	Device
#)

#QT5_ADD_DBUS_INTERFACE(SOURCES
#	resources/org.libmtp.mtpd.folder.xml
#	Folder
#)

#QT5_ADD_DBUS_ADAPTOR(SOURCES
#   resources/org.libmtp.mtpd.file.xml
#	DBusMTPFile.h
#	DBusMTP::File
#	org_libmtp_mtpd_file_adaptor
#	OrgLibMTPMtpdFileAdaptor
#)

QT5_ADD_DBUS_INTERFACE(SOURCES
	resources/org.libmtp.mtpd.file.xml
	File
)


#QT5_ADD_DBUS_INTERFACE(SOURCES
	#resources/org.libmtp.mptd.object.xml
	#Object
#)


QT5_ADD_DBUS_INTERFACE(SOURCES
	resources/org.mate.SettingsDaemon.MediaKeys.xml
	mate_settings_daemon
)

QT5_ADD_DBUS_INTERFACE(SOURCES
	resources/org.gnome.SettingsDaemon.MediaKeys.xml
	gnome_settings_daemon
)

QT5_ADD_DBUS_INTERFACE(SOURCES
	resources/org.gnome.SettingsDaemon.MediaKeys.xml
	gnome_settings_daemon
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_helper
    say_interfaces_notifications
)






