/* TableView.h */

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

#ifndef LIBRARYTABLEVIEW_H
#define LIBRARYTABLEVIEW_H

#include "ItemView.h"
#include "Utils/Library/Sortorder.h"
#include "GUI/Library/Utils/ColumnHeader.h"
#include "Utils/Pimpl.h"

namespace Library
{
	class HeaderView;
	class TableView :
		public Library::ItemView
	{
		Q_OBJECT
		PIMPL(TableView)

	signals:
		void sig_columns_changed();
		void sig_sortorder_changed(Library::SortOrder);

	private:
		TableView(const TableView& other)=delete;
		TableView& operator=(const TableView& other)=delete;

	public:
		explicit TableView(QWidget* parent=nullptr);
		~TableView();

		virtual void init(AbstractLibrary* library);

	protected:
		virtual void init_view(AbstractLibrary* library)=0;
		virtual ColumnHeaderList column_headers() const=0;
		virtual BoolList shown_columns() const=0;
		virtual Library::SortOrder sortorder() const=0;

		void resizeEvent(QResizeEvent* e) override;
		void language_changed() override;

		// SayonaraSelectionView.h
		int index_by_model_index(const QModelIndex& idx) const override;
		QModelIndex model_index_by_index(int idx) const override;

		virtual void columns_changed();
		virtual void sortorder_changed(SortOrder s);


	protected slots:
		void header_actions_triggered();
		void sort_by_column(int column_idx);
	};
}

#endif // LIBRARYTABLEVIEW_H
