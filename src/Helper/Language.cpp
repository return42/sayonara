#include "Language.h"
#include "Helper.h"

LanguageString::LanguageString(const QString& str) :
	QString(str)
{

}

LanguageString LanguageString::toFirstUpper() const
{
	return LanguageString(Helper::cvt_str_to_first_upper(*this));
}

LanguageString LanguageString::space() const
{
	LanguageString str = *this;
	return str + " ";
}

LanguageString LanguageString::question() const
{
	LanguageString str = *this;
	return str + "?";
}

LanguageString LanguageString::triplePt() const
{
	LanguageString str = *this;
	return str + "...";
}

Lang::Lang() {}

Lang::~Lang() {}

LanguageString Lang::get(Lang::Term term)
{
	Lang l;
	switch(term)
	{
		case About:
			return l.tr("About");
		case Action:
			return l.tr("Action");
		case Actions:
			return l.tr("Actions");
		case AddTab:
			return l.tr("Add tab");
		case Album:
			return l.tr("Album");
		case Albums:
			return l.tr("Albums");
		case Append:
			return l.tr("Append");
		case Artist:
			return l.tr("Artist");
		case Artists:
			return l.tr("Artists");
		case Bitrate:
			return l.tr("Bitrate");
		case Bookmarks:
			return l.tr("Bookmarks");
		case Broadcast:
			return l.tr("Broadcast");
		case By:
			return l.tr("by");
		case CannotFindLame:
			return l.tr("Cannot find Lame MP3 encoder");
		case Clear:
			return l.tr("Clear");
		case Close:
			return l.tr("Close");
		case CloseOthers:
			return l.tr("Close others");
		case CloseTab:
			return l.tr("Close tab");
		case Continue:
			return l.tr("Continue");
		case Days:
		case DaysShort:
			return l.tr("d");
		case Default:
			return l.tr("Default");
		case Delete:
			return l.tr("Delete");
		case Directory:
			return l.tr("Directory");
		case Directories:
			return l.tr("Directories");
		case Duration:
			return l.tr("Duration");
		case DurationShort:
			return l.tr("Dur.");
		case DynamicPlayback:
			return l.tr("Dynamic playback");
		case Edit:
			return l.tr("Edit");
		case Entry:
			return l.tr("Entry");
		case Entries:
			return l.tr("Entries");
		case Error:
			return l.tr("Error");
		case Fast:
			return l.tr("Fast");
		case File:
			return l.tr("File");
		case Files:
			return l.tr("Files");
		case Filesize:
			return l.tr("Filesize");
		case First:
			return l.tr("1st");
		case GaplessPlayback:
			return l.tr("Gapless playback");
		case Genre:
			return l.tr("Genre");
		case Genres:
			return l.tr("Genres");
		case Hours:
		case HoursShort:
			return l.tr("h");
		case ImportDir:
			return l.tr("Import directory");
		case ImportFiles:
			return l.tr("Import files");
		case Info:
			return l.tr("Info");
		case Library:
			return l.tr("Library");
		case LibraryPath:
			return l.tr("Library path");
		case Listen:
			return l.tr("Listen");
		case Lyrics:
			return l.tr("Lyrics");
		case Menu:
			return l.tr("Menu");
		case Minutes:
		case MinutesShort:
			return l.tr("m");
		case MuteOn:
			return l.tr("Mute on");
		case MuteOff:
			return l.tr("Mute off");
		case New:
			return l.tr("New");
		case NextTrack:
			return l.tr("Next track");
		case No:
			return l.tr("No");
		case None:
			return l.tr("None");
		case NumTracks:
			return QString("#") + l.tr("Tracks");
		case On:
			return l.tr("on");
		case Open:
			return l.tr("Open");
		case OpenDir:
			return l.tr("Open directory");
		case OpenFile:
			return l.tr("Open file");
		case Overwrite:
			return l.tr("Overwrite");
		case Pause:
			return l.tr("Pause");
		case Play:
			return l.tr("Play");
		case PlayPause:
			return l.tr("Play/Pause");
		case PlayingTime:
			return l.tr("Playing time");
		case Playlist:
			return l.tr("Playlist");
		case Playlists:
			return l.tr("Playlists");
		case PlayNext:
			return l.tr("Play next");
		case PreviousTrack:
			return l.tr("Previous track");
		case Radio:
			return l.tr("Radio");
		case Rating:
			return l.tr("Rating");
		case Really:
			return l.tr("Really");
		case ReloadLibrary:
			return l.tr("Reload library");
		case Remove:
			return l.tr("Remove");
		case Rename:
			return l.tr("Rename");
		case Repeat1:
			return l.tr("Repeat 1");
		case RepeatAll:
			return l.tr("Repeat all");
		case Reset:
			return l.tr("Reset");
		case Refresh:
			return l.tr("Refresh");
		case Sampler:
			return l.tr("Sampler");
		case Shuffle:
			return l.tr("Shuffle");
		case Shutdown:
			return l.tr("Shutdown");
		case Save:
			return l.tr("Save");
		case SaveAs:
			return l.tr("Save as");
		case Search:
			return l.tr("Search");
		case SearchNext:
			return l.tr("Search next");
		case Second:
			return l.tr("2nd");
		case Seconds:
		case SecondsShort:
			return l.tr("s");
		case SeekBackward:
			return l.tr("Seek backward");
		case SeekForward:
			return l.tr("Seek forward");
		case Show:
			return l.tr("Show");
		case SimilarArtists:
			return l.tr("Similar artists");
		case Stop:
			return l.tr("Stop");
		case Success:
			return l.tr("Success");
		case Th:
			return l.tr("th");
		case Third:
			return l.tr("3rd");
		case Title:
			return l.tr("Title");
		case Track:
			return l.tr("track");
		case TrackOn:
			return l.tr("track on");
		case Tracks:
			return l.tr("tracks");
		case Undo:
			return l.tr("Undo");
		case Various:
			return l.tr("Various");
		case VariousAlbums:
			return l.tr("Various albums");
		case VariousArtists:
			return l.tr("Various artists");
		case VariousTracks:
			return l.tr("Various tracks");
		case Version:
			return l.tr("Version");
		case VolumeDown:
			return l.tr("Volume down");
		case VolumeUp:
			return l.tr("Volume up");
		case Warning:
			return l.tr("Warning");
		case Year:
			return l.tr("Year");
		case Yes:
			return l.tr("Yes");
		default:
			return QString();
	}
}