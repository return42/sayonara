
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

#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidgetTemplate.h"
#include "Helper/typedefs.h"
#include "Helper/Library/Sortorder.h"

class HeaderView :
        public SayonaraWidgetTemplate<QHeaderView>
{
	Q_OBJECT

signals:
	void sig_columns_changed();

private:
	QMenu*				_context_menu=nullptr;
	ColumnHeaderList	_column_headers;

private:
	void init_header_action(ColumnHeader* header, bool is_shown);

private slots:
	void action_triggered(bool b);

protected:
	BoolList refresh_active_columns();
    void language_changed() override;

public:

	HeaderView(Qt::Orientation orientation, QWidget* parent=nullptr);
	QSize sizeHint() const override;

	void set_column_headers(const ColumnHeaderList& column_headers, const BoolList& shown_columns, Library::SortOrder sorting );
	void refresh_sizes(QTableView* view);

	BoolList get_shown_columns() const;

	ColumnHeader* get_column_header(int idx);
};

#endif // HEADERVIEW_H
