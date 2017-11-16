/* IconLoader.h */

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

#ifndef ICON_LOADER_H_
#define ICON_LOADER_H_

class QString;
class QStringList;
class QIcon;
class QPixmap;

template <typename T>
class QList;

/**
 * @brief The IconLoader class
 * @ingroup GUIHelper
 */
namespace IconLoader
{
	enum IconMode
	{
		Automatic,
		ForceStdIcon,
		ForceSayonaraIcon
	};

	enum IconName
	{
		Append=0,		// Playlist
		AudioFile,
		Dynamic,
		Repeat1,
		RepeatAll,
		Shuffle,
		Gapless,
		Shutdown,

		Backward,
		Forward,
		LocalLibrary,
		Pause,
		Play,
		PlayBorder,
		PlaySmall,
		Previous,		// Player controls
		Next,
		PlaylistFile,
		Record,
		Stop,
		Vol1,
		Vol2,
		Vol3,
		VolMute,

		Delete,
		Folder,
		FolderOpen,
		FolderVisiting,
		Save,
		SaveAs,

		Clear,
		Close,
		Edit,
		Exit,
		FileManager,
		Image,
		Info,
		New,
		Open,
		Refresh,
		Remove,
		Rename,
		Search,
		Star,
		StarDisabled,
		Table,
		Undo,

		Lyrics
	};

	QIcon icon(IconName name);
	QIcon icon(IconName name, IconMode mode);

	QPixmap pixmap(IconName name);
	QPixmap pixmap(IconName name, IconMode mode);

	void set_standard_theme(const QString& name);
	QString standard_theme();

	void change_theme();
}

#endif
