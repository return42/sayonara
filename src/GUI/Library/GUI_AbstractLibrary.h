/* GUI_AbstractLibrary.h */

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

#ifndef GUI_ABSTRACTLIBRARY_H
#define GUI_ABSTRACTLIBRARY_H

#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

#include "Helper/typedefs.h"
#include "Helper/SetFwd.h"

#include "Helper/Library/Filter.h"
#include "Helper/Library/Sorting.h"
#include "Helper/Library/LibraryNamespaces.h"
#include "Helper/MetaData/MetaDataFwd.h"

#include "Helper/Pimpl.h"

class QPushButton;
class QComboBox;
class AbstractLibrary;
class LibraryTableView;

class GUI_AbstractLibrary :
		public SayonaraWidget
{
	Q_OBJECT
	PIMPL(GUI_AbstractLibrary)

public:
	explicit GUI_AbstractLibrary(AbstractLibrary* library,
								 QWidget *parent=nullptr);

	virtual ~GUI_AbstractLibrary();

protected:
	virtual void init_headers();
	virtual void init_shortcuts();
	virtual void language_changed() override;
	virtual ::Library::TrackDeletionMode show_delete_dialog(int n_tracks)=0;

	virtual QList<::Library::Filter::Mode> search_options() const;

private:
	void init();
	void init_finished();
	void init_search_combobox();

protected slots:
	virtual void _sl_live_search_changed();

protected slots:

	virtual void refresh();

	virtual void lib_fill_tracks(const MetaDataList&);
	virtual void lib_fill_albums(const AlbumList&);
	virtual void lib_fill_artists(const ArtistList&);
	virtual void track_info_available(const MetaDataList& v_md);

	virtual void artist_sel_changed(const SP::Set<int>&);
	virtual void album_sel_changed(const SP::Set<int>&);
	virtual void track_sel_changed(const SP::Set<int>&);

	virtual void artist_middle_clicked(const QPoint& p);
	virtual void album_middle_clicked(const QPoint& p);
	virtual void tracks_middle_clicked(const QPoint& p);

	virtual void artist_dbl_clicked(const QModelIndex &);
	virtual void album_dbl_clicked(const QModelIndex &);
	virtual void track_dbl_clicked(const QModelIndex &);

	virtual void columns_title_changed(const BoolList&);
	virtual void columns_album_changed(const BoolList&);
	virtual void columns_artist_changed(const BoolList&);

	virtual void sortorder_title_changed(::Library::SortOrder);
	virtual void sortorder_album_changed(::Library::SortOrder);
	virtual void sortorder_artist_changed(::Library::SortOrder);

	virtual void text_line_edited(const QString&);
	virtual void clear_button_pressed();
	virtual void combo_search_changed(int idx);
	virtual void return_pressed();

	virtual void delete_artist();
	virtual void delete_album();
	virtual void delete_tracks();

	virtual void refresh_artist();
	virtual void refresh_album();
	virtual void refresh_tracks();

	virtual void album_rating_changed(int);
	virtual void title_rating_changed(int);

	virtual void append();
	virtual void append_tracks();
	virtual void play_next();
	virtual void play_next_tracks();

	virtual void show_delete_answer(QString);


public slots:
	virtual void id3_tags_changed();

protected:

	virtual LibraryTableView* lv_artist() const=0;
	virtual LibraryTableView* lv_album() const=0;
	virtual LibraryTableView* lv_tracks() const=0;
	virtual QPushButton* btn_clear() const=0;
	virtual QLineEdit* le_search() const=0;
	virtual QComboBox* combo_search() const=0;

	template<typename T, typename UI>
	void setup_parent(T* subclass, UI** ui)
	{
		*ui = new UI();

		UI* ui_ptr = *ui;
		ui_ptr->setupUi(subclass);

		init();
	}
};

#endif // GUI_ABSTRACTLIBRARY_H
