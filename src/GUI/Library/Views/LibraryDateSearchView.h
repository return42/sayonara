/* LibraryDateSearchView.h */

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



#ifndef LIBRARYDATESEARCHVIEW_H
#define LIBRARYDATESEARCHVIEW_H

#include "GUI/Helper/SearchableWidget/SearchableListView.h"
#include "Helper/Pimpl.h"

#include <QContextMenuEvent>

namespace Library { class DateFilter; }

class LibraryDateSearchView :
		public SearchableListView
{
	Q_OBJECT

	PIMPL(LibraryDateSearchView)

public:
	explicit LibraryDateSearchView(QWidget* parent=nullptr);
	~LibraryDateSearchView();

	Library::DateFilter get_filter(int row) const;

protected:
	void contextMenuEvent(QContextMenuEvent* e) override;

protected slots:
	void new_clicked();
	void edit_clicked();
	void delete_clicked();
};


#endif // LIBRARYDATESEARCHVIEW_H
