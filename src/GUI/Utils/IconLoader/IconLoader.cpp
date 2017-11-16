/* IconLoader.cpp */

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

#include "IconLoader.h"

#include "Components/DirectoryReader/DirectoryReader.h"
#include "GUI/Utils/GuiUtils.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"

#include <QIcon>
#include <QMap>
#include <QPair>

static char* s_standard_theme=nullptr;

using P=QPair<QString, QString>;

static QMap<IconLoader::IconName, QPair<QString, QString>> s_icon_map =
{
	{IconLoader::Append,	P("list-add", "append")},
	{IconLoader::Backward,	P("media-skip-backward", "bwd")},
	{IconLoader::Clear,		P("edit-clear", "broom.png")},
	{IconLoader::Close,		P("window-close", "close")},
	{IconLoader::Delete,	P("edit-delete", "delete")},
	{IconLoader::Dynamic,	P("dynamic", "dynamic")},
	{IconLoader::Edit,		P("accessories-text-editor", "edit")},
	{IconLoader::Exit,		P("application-exit", "close")},
	{IconLoader::FileManager, P("system-file-manager", "folder")},
	{IconLoader::Forward,	P("media-skip-forward", "fwd")},
	{IconLoader::Gapless,	P("gapless", "gapless")},
	{IconLoader::Folder,	P("folder", "folder")},
	{IconLoader::FolderOpen,P("folder-open", "folder_open")},
	{IconLoader::FolderVisiting,P("folder-visiting", "folder_open")},
	{IconLoader::Image,		P("image-x-generic", "")},
	{IconLoader::Info,		P("dialog-information", "info")},
	{IconLoader::LocalLibrary, P("audio-x-generic", "append.svg")},
	{IconLoader::Lyrics,	P("document-properties", "lyrics")},
	{IconLoader::New,		P("document-new", "new")},
	{IconLoader::Next,		P("media-skip-forward", "fwd")},
	{IconLoader::Open,		P("document-open", "open")},
	{IconLoader::Pause,		P("media-playback-pause", "pause")},
	{IconLoader::Play,		P("media-playback-start", "play")},
	{IconLoader::PlaySmall,	P("media-playback-start", "play_small")},
	{IconLoader::PlayBorder,P("media-playback-start", "play_bordered")},
	{IconLoader::PlaylistFile, P("text-x-generic", "playlistfile")},
	{IconLoader::Previous,	P("media-skip-backward", "bwd")},
	{IconLoader::Record,	P("media-record", "rec")},
	{IconLoader::Refresh,	P("view-refresh", "undo")},
	{IconLoader::Remove,	P("list-remove", "remove")},
	{IconLoader::Rename,	P("accessories-text-editor", "edit")},
	{IconLoader::Repeat1,	P("rep_1", "rep_1")},
	{IconLoader::RepeatAll,	P("rep_all", "rep_all")},
	{IconLoader::Save,		P("document-save", "save")},
	{IconLoader::SaveAs,	P("document-save-as", "save_as")},
	{IconLoader::Search,	P("edit-find", "lupe")},
	{IconLoader::Shuffle,	P("shuffle", "shuffle")},
	{IconLoader::Shutdown,	P("power_on", "power_on")},
	{IconLoader::Star,		P("rating", "star.png")},
	{IconLoader::StarDisabled, P("star_disabled.png", "star_disabled.png")},
	{IconLoader::Stop,		P("media-playback-stop", "stop")},
	{IconLoader::Table,		P("format-justify-fill", "")},
	{IconLoader::Undo,		P("edit-undo", "undo")},
	{IconLoader::Vol1,		P("audio-volume-low", "vol_1_dark")},
	{IconLoader::Vol2,		P("audio-volume-medium", "vol_2_dark")},
	{IconLoader::Vol3,		P("audio-volume-high", "vol_3_dark")},
	{IconLoader::VolMute,	P("audio-volume-muted", "vol_mute_dark")},
};


#ifdef Q_OS_WIN
QString get_win_icon_name(const QString& name)
{
	QString icon_name = QString(":/IconsWindows/") + name + ".png";
	return icon_name;
}
#endif

QIcon IconLoader::icon(IconLoader::IconName spec, IconLoader::IconMode mode)
{
	bool dark = (Settings::instance()->get(Set::Player_Style) == 1);
	QString std_name = s_icon_map[spec].first;
	QString dark_name = s_icon_map[spec].second;

	QIcon icon;

	if(mode == IconMode::ForceSayonaraIcon){
		icon = Gui::Util::icon(dark_name);
	}

	else if(mode == IconMode::ForceStdIcon){
		icon = QIcon::fromTheme(std_name);
	}

	if(icon.isNull())
	{
		if(!dark)
		{
	#ifdef Q_OS_WIN
			icon = QIcon(get_win_icon_name(std_name));
	#else

			icon = QIcon::fromTheme(std_name);
	#endif
		}

		else
		{
	#ifdef Q_OS_WIN
			icon = QIcon(get_win_icon_name(std_name));
	#else
			icon = Gui::Util::icon(dark_name);
	#endif
		}
	}

	if(!icon.isNull())
	{
		return icon;
	}

	return Gui::Util::icon(dark_name);
}


QIcon IconLoader::icon(IconName spec)
{
	return icon(spec, IconMode::Automatic);
}


void IconLoader::change_theme()
{
	Settings* s = Settings::instance();
	QString theme = s->get(Set::Icon_Theme);

	QIcon::setThemeName(theme);
}


QPixmap IconLoader::pixmap(IconLoader::IconName spec)
{
	return pixmap(spec, IconLoader::Automatic);
}

QPixmap IconLoader::pixmap(IconLoader::IconName spec, IconLoader::IconMode mode)
{
	bool dark = (Settings::instance()->get(Set::Player_Style) == 1);
	QString std_name = s_icon_map[spec].first;
	QString dark_name = s_icon_map[spec].second;

	QPixmap pm;

	if(mode == IconMode::ForceSayonaraIcon){
		pm = Gui::Util::pixmap(dark_name);
	}

	else if(mode == IconMode::ForceStdIcon){
		pm = QIcon::fromTheme(std_name).pixmap(QSize(32,32));
	}

	if(pm.isNull())
	{
		if(!dark)
		{
	#ifdef Q_OS_WIN
			pm = QIcon(get_win_icon_name(std_name)).pixmap(QSize(32,32));
	#else

			pm = QIcon::fromTheme(std_name).pixmap(QSize(32,32));
	#endif
		}

		else
		{
	#ifdef Q_OS_WIN
			pm = QIcon(get_win_icon_name(std_name)).pixmap(QSize(32,32));
	#else
			pm = Gui::Util::pixmap(dark_name);
	#endif
		}
	}

	if(!pm.isNull())
	{
		return pm;
	}

	return Gui::Util::pixmap(dark_name);
}

void IconLoader::set_standard_theme(const QString& name)
{
	s_standard_theme = strdup(name.toLocal8Bit().data());
}

QString IconLoader::standard_theme()
{
	return QString(s_standard_theme);
}
