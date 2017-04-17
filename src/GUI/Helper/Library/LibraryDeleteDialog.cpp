/* LibraryDeleteDialog.cpp */

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



#include "LibraryDeleteDialog.h"
#include "Helper/Language.h"
#include <QPushButton>

LibraryDeleteDialog::LibraryDeleteDialog(int n_tracks, QWidget* parent) :
    QMessageBox(parent)
{
    _answer = Library::TrackDeletionMode::None;
    _n_tracks = n_tracks;
}

LibraryDeleteDialog::~LibraryDeleteDialog() {}

int LibraryDeleteDialog::exec()
{
    _answer = Library::TrackDeletionMode::None;

    this->setFocus();
    this->setIcon(QMessageBox::Warning);
    this->setText("<b>" + Lang::get(Lang::Warning) + "!</b>");
    this->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QPushButton* only_library_button = this->addButton(tr("Only from library"), QMessageBox::AcceptRole);
    this->setDefaultButton(QMessageBox::No);

    QString info_text = tr("You are about to delete %1 files").arg(_n_tracks);
    this->setInformativeText(info_text + "\n" + Lang::get(Lang::Continue).question() );

    int ret = QMessageBox::exec();
    QAbstractButton* clicked_button = this->clickedButton();

    if(ret == QMessageBox::No){
	_answer = Library::TrackDeletionMode::None;
    }

    else if(ret == QMessageBox::Yes){
	_answer = Library::TrackDeletionMode::AlsoFiles;
    }

    else if(clicked_button->text() == only_library_button->text()) {
	_answer = Library::TrackDeletionMode::OnlyLibrary;
    }

    else{
	_answer = Library::TrackDeletionMode::None;
    }

    return ret;
}

Library::TrackDeletionMode LibraryDeleteDialog::answer() const
{
    return _answer;
}

void LibraryDeleteDialog::set_num_tracks(int n_tracks)
{
    _n_tracks = n_tracks;
}

