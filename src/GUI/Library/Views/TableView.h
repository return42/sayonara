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

#include "View.h"
#include "Helper/Library/Sortorder.h"
#include "Helper/Pimpl.h"

namespace Library
{
	class HeaderView;
	class TableView :
			public Library::View
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

		void set_table_headers(const ColumnHeaderList& headers, const BoolList& shown_columns, Library::SortOrder sorting);
		BoolList get_shown_columns() const;

		int get_index_by_model_index(const QModelIndex& idx) const override;
		QModelIndex get_model_index_by_index(int idx) const override;

	protected:
		HeaderView*	get_header_view();
		void resizeEvent(QResizeEvent* e) override;
		void language_changed() override;

	protected slots:
		void header_actions_triggered();
		void sort_by_column(int column_idx);

	};
}

#endif // LIBRARYTABLEVIEW_H
