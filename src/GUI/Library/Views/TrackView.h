/* TrackView.h */

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



#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include "TableView.h"
#include "Utils/Pimpl.h"
#include "Utils/Library/Sortorder.h"

class AbstractLibrary;
namespace Library
{
	class TrackView :
		public Library::TableView
	{
		Q_OBJECT
		PIMPL(TrackView)

	public:
		explicit TrackView(QWidget* parent=nullptr);
		~TrackView();

	private:
		//from Library::TableView
		void init_view(AbstractLibrary* library) override;
		ColumnHeaderList column_headers() const override;
		BoolList shown_columns() const override;
		SortOrder sortorder() const override;

		void sortorder_changed(SortOrder s) override;
		void columns_changed() override;

		// from Library::ItemView
		void play_clicked() override;
		void play_new_tab_clicked() override;
		void middle_clicked() override;
		void play_next_clicked() override;
		void append_clicked() override;
		void selection_changed(const IndexSet& lst) override;
		void refresh_clicked() override;

	private slots:
		void double_clicked(const QModelIndex& idx);
		void tracks_ready();
	};
}

#endif // TRACKVIEW_H
