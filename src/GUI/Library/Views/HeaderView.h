
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

#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>
#include <QWidget>
#include <QMenu>
#include <QTableView>

#include "GUI/Library/Utils/ColumnHeader.h"
#include "GUI/Utils/Widgets/WidgetTemplate.h"
#include "Utils/Library/Sortorder.h"
#include "Utils/Pimpl.h"

namespace Library
{
	class HeaderView :
			public Gui::WidgetTemplate<QHeaderView>
	{
		Q_OBJECT
		PIMPL(HeaderView)

	signals:
		void sig_columns_changed();

	private:
		void init_header_action(ColumnHeader* header, bool is_shown);

	private slots:
		void action_triggered(bool b);

	protected:
		BoolList refresh_active_columns();
		void language_changed() override;

	public:
		HeaderView(Qt::Orientation orientation, QWidget* parent=nullptr);
		virtual ~HeaderView();

		QSize sizeHint() const override;

		void set_column_headers(const ColumnHeaderList& column_headers, const BoolList& shown_columns, Library::SortOrder sorting );
		void refresh_sizes(QTableView* view);

		BoolList shown_columns() const;
		ColumnHeader* column_header(int idx);
	};
}

#endif // HEADERVIEW_H
