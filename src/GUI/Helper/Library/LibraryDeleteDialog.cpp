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

