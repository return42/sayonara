/* GUI_LocalLibrary.h */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
 * GUI_LocalLibrary.h
 *
 *  Created on: Apr 24, 2011
 *      Author: Lucio Carreras
 */

#ifndef GUI_LOCAL_LIBRARY_H_
#define GUI_LOCAL_LIBRARY_H_

#include "GUI_AbstractLibrary.h"

class GUI_LibraryInfoBox;
class GUI_ImportFolder;
class LocalLibraryMenu;
class AlbumCoverView;
class AlbumCoverModel;

namespace Ui { class GUI_LocalLibrary; }

class GUI_LocalLibrary :
		public GUI_AbstractLibrary
{

	Q_OBJECT

public:

	explicit GUI_LocalLibrary(QWidget* parent=nullptr);
	virtual ~GUI_LocalLibrary();

	QComboBox*	get_libchooser() const;
	QMenu*		get_menu() const;


protected:
	void showEvent(QShowEvent* e) override;
	void init_shortcuts() override;

	Library::ReloadQuality show_quality_dialog();


private slots:

	void switch_album_view();

	void disc_pressed(int disc);
	void lib_no_lib_path();
	void progress_changed(const QString& type, int progress);

	void genre_selection_changed(const QModelIndex& index);
	void date_selection_changed(const QModelIndex& index);

	void reload_library_requested();
	void import_dirs_requested();
	void import_files_requested();
	void import_files(const QStringList& files);

	// importer requests dialog
	void import_dialog_requested();

	// set library path clicked
	void set_library_path_clicked();

	void splitter_artist_moved(int pos, int idx);
	void splitter_tracks_moved(int pos, int idx);
	void splitter_genre_moved(int pos, int idx);
	void splitter_date_moved(int pos, int idx);

	// reimplemented from Abstract Library
	Library::TrackDeletionMode show_delete_dialog(int n_tracks) override;
	void reload_finished();

	void show_info_box();

	// reimplemented from Sayonara widget
	void language_changed() override;

	void _sl_libpath_changed();
	void clear_button_pressed() override;


protected slots:
	void lib_fill_albums(const AlbumList& albums) override;
	void lib_fill_tracks(const MetaDataList& v_md) override;

private:
	Ui::GUI_LocalLibrary*	ui=nullptr;
	GUI_LibraryInfoBox*		_library_info_box=nullptr;
	GUI_ImportFolder*		_ui_importer=nullptr;
	LocalLibraryMenu*		_local_library_menu=nullptr;

	AlbumCoverView*			_acv = nullptr;
	AlbumCoverModel*		_acm = nullptr;

	void init_album_cover_view();
};


#endif /* GUI_LocalLibrary_H_ */

