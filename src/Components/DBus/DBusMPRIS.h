/* DBusMPRIS.h */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DBUS_MPRIS_H
#define DBUS_MPRIS_H

#include <QObject>
#include <QVariant>
#include <QDBusObjectPath>
#include <QMainWindow>

#include "Components/PlayManager/PlayState.h"

#include "Utils/MetaData/MetaData.h"
#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Pimpl.h"

using QStrRef=const QString&;

class DBusAdaptor :
		public QObject
{
	Q_OBJECT
    PIMPL(DBusAdaptor)

protected:
    explicit DBusAdaptor(QStrRef object_path, QStrRef service_name, QStrRef dbus_service, QStrRef dbus_interface, QObject *parent=nullptr);
	virtual ~DBusAdaptor();

	void create_message(QString name, QVariant val);

    QString object_path() const;
    QString service_name() const;
    QString dbus_service() const;
    QString dbus_interface() const;
};


namespace DBusMPRIS 
{

class MediaPlayer2 :
		public DBusAdaptor,
		public SayonaraClass
{
	Q_OBJECT
    PIMPL(MediaPlayer2)

	public:
		explicit MediaPlayer2(QMainWindow* player, QObject *parent=nullptr);
		~MediaPlayer2();

		Q_PROPERTY(bool			CanQuit				READ CanQuit)
		bool					CanQuit();

		Q_PROPERTY(bool			CanRaise			READ CanRaise)
		bool					CanRaise();

		Q_PROPERTY(bool			HasTrackList		READ HasTrackList)
		bool					HasTrackList();


		Q_PROPERTY(QString		Identity			READ Identity)
		QString					Identity();

		Q_PROPERTY(QString		DesktopEntry		READ DesktopEntry)
		QString					DesktopEntry();

		Q_PROPERTY(QStringList	SupportedUriSchemes	READ SupportedUriSchemes)
		QStringList				SupportedUriSchemes();


		Q_PROPERTY(QStringList	SupportedMimeTypes	READ SupportedMimeTypes)
		QStringList				SupportedMimeTypes();


		Q_PROPERTY(bool			CanSetFullscreen	READ CanSetFullscreen)
		bool					CanSetFullscreen();

		Q_PROPERTY(bool			Fullscreen			READ Fullscreen				WRITE SetFullscreen)
		bool					Fullscreen();
		void					SetFullscreen(bool b);

		void					Raise();
		void					Quit();


	private:
		void			init();


	public:
		Q_PROPERTY(QString		PlaybackStatus		READ	PlaybackStatus)
		QString					PlaybackStatus();


		Q_PROPERTY(QString		LoopStatus			READ	LoopStatus	WRITE	SetLoopStatus)
		QString					LoopStatus();
		void					SetLoopStatus(QString status);


		Q_PROPERTY(double		Rate				READ	Rate		WRITE	SetRate)
		double					Rate();
		void					SetRate(double rate);


		Q_PROPERTY(bool			Shuffle				READ	Shuffle		WRITE	SetShuffle)
		bool					Shuffle();
		void					SetShuffle(bool shuffle);


		Q_PROPERTY(QVariantMap	Metadata			READ	Metadata)
		QVariantMap				Metadata();


		Q_PROPERTY(double		Volume				READ	Volume		WRITE	SetVolume)
		double					Volume();
		void					SetVolume(double volume);


		Q_PROPERTY(int64_t	Position			READ	Position)
		int64_t				Position();
		void				SetPosition(const QDBusObjectPath& track_id, int64_t position);


		Q_PROPERTY(double		MinimumRate			READ	MinimumRate)
		double					MinimumRate();


		Q_PROPERTY(double		MaximumRate			READ	MaximumRate)
		double					MaximumRate();


		Q_PROPERTY(bool			CanGoNext			READ	CanGoNext)
		bool					CanGoNext();


		Q_PROPERTY(bool			CanGoPrevious		READ	CanGoPrevious)
		bool					CanGoPrevious();


		Q_PROPERTY(bool			CanPlay				READ	CanPlay)
		bool					CanPlay();


		Q_PROPERTY(bool			CanPause			READ	CanPause)
		bool					CanPause();


		Q_PROPERTY(bool			CanSeek				READ	CanSeek)
		bool					CanSeek();


		Q_PROPERTY(bool			CanControl			READ	CanControl)
		bool					CanControl();


		void					Next();
		void					Previous();
		void					Pause();
		void					PlayPause();
		void					Stop();
		void					Play();
		void					Seek(int64_t offset);

		void					OpenUri(const QString& uri);


	public slots:

		void					position_changed(uint64_t pos_ms);
		void					volume_changed(int volume);
		void					track_idx_changed(int idx);
		void					playlist_len_changed(int len);
		void					track_changed(const MetaData& md);
		void					playstate_changed(PlayState state);

	signals:
		void					Seeked(int64_t position);
		void					sig_raise();
};
} // end namespace DBusMPRIS

#endif // DBUS_MPRIS_H
