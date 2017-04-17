/* LibraryDeleteDialog.h */

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



#ifndef LIBRARYDELETEDIALOG_H
#define LIBRARYDELETEDIALOG_H

#include <QMessageBox>
#include "Helper/Library/LibraryNamespaces.h"

class LibraryDeleteDialog :
	public QMessageBox
{
private:
    Library::TrackDeletionMode _answer;
    int	_n_tracks;

public:
    LibraryDeleteDialog(int n_tracks, QWidget* parent=nullptr);
    virtual ~LibraryDeleteDialog();

    void set_num_tracks(int n_tracks);

    int exec() override;
    Library::TrackDeletionMode answer() const;
};


#endif // LIBRARYDELETEDIALOG_H
