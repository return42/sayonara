/* Language.h */

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

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QObject>

class LanguageString : public QString
{
public:
	LanguageString(const QString& str);
	LanguageString toFirstUpper() const;
	LanguageString space() const;
	LanguageString question() const;
	LanguageString triplePt() const;
};

class Lang :
		public QObject
{
	Q_OBJECT

public:
	enum Term
	{
		About=0,
		Action,
		Actions,
		AddTab,
		Album,
		AlbumArtists,
		Albums,
		Append,
		Artist,
		Artists,
		Bitrate,
		Bookmarks,
		Broadcast,
		By,
		CannotFindLame,
		Continue,
		Clear,
		Close,
		CloseOthers,
		CloseTab,
		Days,
		DaysShort,
		Default,
		Delete,
		Directory,
		Directories,
		Duration,
		DurationShort,
		DynamicPlayback,
		Edit,
		Entries,
		Entry,
		Error,
		Fast,
		File,
		Files,
		Filesize,
		First,
		Font,
		Fonts,
		GaplessPlayback,
		Genre,
		Genres,
		Hours,
		HoursShort,
		ImportDir,
		ImportFiles,
		Info,
		Library,
		LibraryPath,
		Listen,
		Lyrics,
		Menu,
		Minutes,
		MinutesShort,
		MuteOn,
		MuteOff,
		New,
		NextTrack,
		No,
		None,
		NumTracks,
		On,
		Open,
		OpenDir,
		OpenFile,
		Overwrite,
		Pause,
		Play,
		PlayPause,
		PlayingTime,
		Playlist,
		Playlists,
		PlayNext,
		PreviousTrack,
		Radio,
		Rating,
		Really,
		ReloadLibrary,
		Reset,
		Refresh,
		Remove,
		Rename,
		Repeat1,
		RepeatAll,
		Sampler,
		Save,
		SaveAs,
		Search,
		SearchNext,
		Second,
		Seconds,
		SecondsShort,
		SeekForward,
		SeekBackward,
		Show,
		ShowAlbumArtists,
		Shuffle,
		Shutdown,
		SimilarArtists,
		Stop,
		Success,
		Th,
		Third,
		Title,
		Track,
		TrackOn,
		Tracks,
		Undo,
		Various,
		VariousAlbums,
		VariousArtists,
		VariousTracks,
		Version,
		VolumeDown,
		VolumeUp,
		Warning,
		Year,
		Yes,
		Zoom,
		NUMBER_OF_LANGUAGE_KEYS
	};

public:
	Lang();
	~Lang();

	static LanguageString get(Lang::Term term, bool* ok=nullptr);
};

#endif // LANGUAGE_H
