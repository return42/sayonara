/* GUI_SoundCloudLibrary.h */

/* Copyright (C) 2011-2016  Lucio Carreras
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



#ifndef GUI_SOUNDCLOUDLIBRARY_H
#define GUI_SOUNDCLOUDLIBRARY_H

#include "ui_GUI_SoundcloudLibrary.h"

#include "GUI/Library/GUI_AbstractLibrary.h"
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"

#include "src/SoundcloudLibrary.h"
#include "ui/GUI_SoundcloudArtistSearch.h"

class GUI_SoundCloudLibrary :
		public GUI_AbstractLibrary,
		protected Ui::GUI_SoundcloudLibrary
{
	friend class GUI_AbstractLibrary;
	Q_OBJECT


public:
	explicit GUI_SoundCloudLibrary(SoundcloudLibrary* library, QWidget *parent=nullptr);

	QComboBox*	get_libchooser() const;
	QMenu*		get_menu() const;

protected:
	virtual AbstractLibrary::TrackDeletionMode show_delete_dialog(int n_tracks);

protected slots:
	void btn_add_clicked();


private:
	GUI_SoundcloudArtistSearch* _artist_search=nullptr;
	QMenu*						_library_menu=nullptr;

};


class SoundcloudLibraryContainer :
	public LibraryContainerInterface
{

	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.sayonara-player.soundcloud_library")
	Q_INTERFACES(LibraryContainerInterface)


private:
	GUI_SoundCloudLibrary*	ui=nullptr;

public:

	SoundcloudLibraryContainer(QObject* parent=nullptr);

	// override from LibraryViewInterface
	QString			get_name() const override;
	QString			get_display_name() const override;
	QIcon			get_icon() const override;
	QWidget*		get_ui() const override;
	QComboBox*		get_libchooser() override;
	QMenu*			get_menu() override;
	void			init_ui() override;

};


#endif // GUI_SOUNDCLOUDLIBRARY_H
