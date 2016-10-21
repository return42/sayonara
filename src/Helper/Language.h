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
		About,
		Action,
		Actions,
		AddTab,
		Album,
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
		Yes
	};

public:
	Lang();
	virtual ~Lang();

	static LanguageString get(Lang::Term term);
};

#endif // LANGUAGE_H
