/* GUI_LocalLibrary.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * GUI_LocalLibrary.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include "GUI_LocalLibrary.h"
#include "LocalLibraryMenu.h"
#include "InfoBox/GUI_LibraryInfoBox.h"
#include "ImportFolderDialog/GUI_ImportFolder.h"
#include "GUI/Helper/GlobalMessage/Message.h"


#include <QFileDialog>
#include <QDir>
#include <QTimer>
#include <QShortcut>


GUI_LocalLibrary::GUI_LocalLibrary(QWidget* parent) :
	GUI_AbstractLibrary(LocalLibrary::getInstance(), parent),
	Ui::GUI_LocalLibrary()
{
	setup_parent(this);

	LocalLibrary* library = LocalLibrary::getInstance();

	_local_library_menu = new LocalLibraryMenu(this);

	pb_progress->setVisible(false);
	lab_progress->setVisible(false);

	connect(_library, &LocalLibrary::sig_reloading_library, this, &GUI_LocalLibrary::progress_changed);
	connect(_library, &LocalLibrary::sig_reloading_library_finished, this, &GUI_LocalLibrary::reload_finished);
	connect(btn_setLibrary, &QPushButton::clicked, this, &GUI_LocalLibrary::set_library_path_clicked);

	connect(lv_album, &LibraryViewAlbum::sig_disc_pressed, this, &GUI_LocalLibrary::disc_pressed);
	connect(lv_album, &LibraryViewAlbum::sig_import_files, this, &GUI_LocalLibrary::import_files);
	connect(lv_artist, &LibraryView::sig_import_files, this, &GUI_LocalLibrary::import_files);
	connect(tb_title, &LibraryView::sig_import_files, this, &GUI_LocalLibrary::import_files);
	connect(lv_genres, &QAbstractItemView::clicked, this, &GUI_LocalLibrary::genre_selection_changed);
	connect(lv_genres, &QAbstractItemView::activated, this, &GUI_LocalLibrary::genre_selection_changed);
	connect(lv_genres, &LibraryGenreView::sig_progress, this, &GUI_LocalLibrary::progress_changed);

	connect(_local_library_menu, &LocalLibraryMenu::sig_reload_library, this, &GUI_LocalLibrary::reload_library_requested);
	connect(_local_library_menu, &LocalLibraryMenu::sig_import_file, this, &GUI_LocalLibrary::import_files_requested);
	connect(_local_library_menu, &LocalLibraryMenu::sig_import_folder, this, &GUI_LocalLibrary::import_dirs_requested);
	connect(_local_library_menu, &LocalLibraryMenu::sig_info, this, &GUI_LocalLibrary::show_info_box);
	connect(btn_reload_library, &QPushButton::clicked, this, &GUI_LocalLibrary::reload_library_requested);

	connect(splitter_artist_album, &QSplitter::splitterMoved, this, &GUI_LocalLibrary::splitter_artist_moved);
	connect(splitter_tracks, &QSplitter::splitterMoved, this, &GUI_LocalLibrary::splitter_tracks_moved);
	connect(splitter_genre, &QSplitter::splitterMoved, this, &GUI_LocalLibrary::splitter_genre_moved);

	connect(library, &LocalLibrary::sig_no_library_path, this, &GUI_LocalLibrary::lib_no_lib_path);
	connect(library, &LocalLibrary::sig_import_dialog_requested, this, &GUI_LocalLibrary::import_dialog_requested);

	setAcceptDrops(true);

	QTimer::singleShot(0, library, SLOT(load()));

	if(lv_genres->get_row_count() <= 1){
		stacked_genre_widget->setCurrentIndex(1);
	}

	else{
		stacked_genre_widget->setCurrentIndex(0);
	}

	REGISTER_LISTENER(Set::Lib_Path, _sl_libpath_changed);
}


GUI_LocalLibrary::~GUI_LocalLibrary() {

}

QComboBox* GUI_LocalLibrary::get_libchooser() const
{
	return combo_lib_chooser;
}

QMenu* GUI_LocalLibrary::get_menu() const
{
	return _local_library_menu;
}

void GUI_LocalLibrary::showEvent(QShowEvent* e)
{
	GUI_AbstractLibrary::showEvent(e);

	QByteArray artist_splitter_state, track_splitter_state, genre_splitter_state;

	artist_splitter_state = _settings->get(Set::Lib_SplitterStateArtist);
	track_splitter_state = _settings->get(Set::Lib_SplitterStateTrack);
	genre_splitter_state = _settings->get(Set::Lib_SplitterStateGenre);

	if(!artist_splitter_state.isEmpty()){
		splitter_artist_album->restoreState(artist_splitter_state);
	}

	if(!track_splitter_state.isEmpty()){
		splitter_tracks->restoreState(track_splitter_state);
	}

	if(!genre_splitter_state.isEmpty()){
		splitter_genre->restoreState(genre_splitter_state);
	}
}

void GUI_LocalLibrary::init_shortcuts()
{
	le_search->setShortcutEnabled(QKeySequence::Find, true);
	new QShortcut(QKeySequence("Esc"), this, SLOT(clear_button_pressed()));
	new QShortcut(QKeySequence("Ctrl+f"), le_search, SLOT(setFocus()));
}


void GUI_LocalLibrary::language_changed() {

	retranslateUi(this);

	GUI_AbstractLibrary::language_changed();
}


void GUI_LocalLibrary::_sl_libpath_changed()
{
	QString library_path = _settings->get(Set::Lib_Path);
	if(!library_path.isEmpty()){
		stacked_widget->setCurrentIndex(0);
	}

	else{
		stacked_widget->setCurrentIndex(1);
	}

	combo_searchfilter->setVisible(!library_path.isEmpty());
	le_search->setVisible(!library_path.isEmpty());
	btn_clear->setVisible(!library_path.isEmpty());
}


void GUI_LocalLibrary::paths_activated(const QStringList& paths)
{
	_library->psl_prepare_tracks_for_playlist(paths, false);
}


void GUI_LocalLibrary::genre_selection_changed(const QModelIndex& index){
	QVariant data = index.data();
	combo_searchfilter->setCurrentIndex(1);
	le_search->setText(data.toString());
	text_line_edited(data.toString());
}



LocalLibrary::TrackDeletionMode GUI_LocalLibrary::show_delete_dialog(int n_tracks) {

	QMessageBox dialog(this);
	QAbstractButton* clicked_button;
	QPushButton* only_library_button;

	dialog.setFocus();
	dialog.setIcon(QMessageBox::Warning);
	dialog.setText("<b>" + tr("Warning") + "!</b>");
	dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	only_library_button = dialog.addButton(tr("Only from library"), QMessageBox::AcceptRole);
	dialog.setDefaultButton(QMessageBox::No);
	QString info_text = tr("You are about to delete %1 files").arg(n_tracks);

	dialog.setInformativeText(info_text + "\n" + tr("Continue?") );

	int answer = dialog.exec();
	clicked_button = dialog.clickedButton();
	dialog.close();


	if(answer == QMessageBox::No){
		return LocalLibrary::TrackDeletionMode::None;
	}

	if(answer == QMessageBox::Yes){
		return LocalLibrary::TrackDeletionMode::AlsoFiles;
	}

	if(clicked_button->text() == only_library_button->text()) {
		return LocalLibrary::TrackDeletionMode::OnlyLibrary;
	}

	return LocalLibrary::TrackDeletionMode::None;
}


void GUI_LocalLibrary::disc_pressed(int disc) {
	LocalLibrary* ll = dynamic_cast<LocalLibrary*>(_library);
	ll->psl_disc_pressed(disc);
}


void GUI_LocalLibrary::lib_no_lib_path(){

	Message::warning(tr("Please select your library path first and reload again."));

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),	QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(dir.size() > 3){
		_settings->set(Set::Lib_Path, dir);
	}
}

void GUI_LocalLibrary::progress_changed(const QString& type, int progress)
{

	pb_progress->setVisible(progress >= 0);
	lab_progress->setVisible(progress >= 0);

	lab_progress->setText(type);

	if(progress == 0){
		if(pb_progress->maximum() != 0){
			pb_progress->setMaximum(0);
		}
	}

	if(progress > 0){
		if(pb_progress->maximum() != 100){
			pb_progress->setMaximum(100);
		}

		pb_progress->setValue(progress);
	}
}


void GUI_LocalLibrary::reload_library_requested()
{
	_library->psl_reload_library(false, Tagging::Quality::Standard);
	btn_reload_library->setEnabled(false);
}


void GUI_LocalLibrary::reload_finished()
{
	btn_reload_library->setEnabled(true);
	lv_genres->reload_genres();

	if(lv_genres->get_row_count() <= 1){
		stacked_genre_widget->setCurrentIndex(1);
	}
	else{
		stacked_genre_widget->setCurrentIndex(0);
	}
}

void GUI_LocalLibrary::show_info_box()
{
	if(!_library_info_box){
		_library_info_box = new GUI_LibraryInfoBox(this);
	}

	_library_info_box->psl_refresh();
}


void GUI_LocalLibrary::import_dirs_requested()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(!dir.isEmpty()) {
		QStringList files = QStringList() << dir;
		_library->import_files(files);
	}
}

void GUI_LocalLibrary::import_files_requested()
{
	QStringList extensions = Helper::get_soundfile_extensions();
	QString filter = QString("Soundfiles (") + extensions.join(" ") + ")";
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Import Files"),
					QDir::homePath(), filter);

	if(files.size() > 0) {
		_library->import_files(files);
	}
}


void GUI_LocalLibrary::import_files(const QStringList& files){
	_library->import_files(files);
}


void GUI_LocalLibrary::import_dialog_requested(){

	if(!_ui_importer){
		_ui_importer = new GUI_ImportFolder(this, true);
	}

	_ui_importer->show();
}


void GUI_LocalLibrary::set_library_path_clicked() {

	QString start_dir = QDir::homePath();
	QString old_dir = _settings->get(Set::Lib_Path);

	if (old_dir.size() > 0 && QFile::exists(old_dir)) {
		start_dir = old_dir;
	}

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			old_dir, QFileDialog::ShowDirsOnly);

	if (dir.size() > 0 && (old_dir.compare(dir) != 0)) {

		_settings->set(Set::Lib_Path, dir);

		GlobalMessage::Answer answer = Message::question_yn(tr("Do you want to reload the Library?"), "Library");

		if(answer == GlobalMessage::Answer::Yes){
			_library->psl_reload_library(false, Tagging::Quality::Standard);
		}
	}
}


void GUI_LocalLibrary::splitter_artist_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	QByteArray arr = splitter_artist_album->saveState();
	_settings->set(Set::Lib_SplitterStateArtist, arr);
}

void GUI_LocalLibrary::splitter_tracks_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	QByteArray arr = splitter_tracks->saveState();
	_settings->set(Set::Lib_SplitterStateTrack, arr);
}

void GUI_LocalLibrary::splitter_genre_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	QByteArray arr = splitter_genre->saveState();
	_settings->set(Set::Lib_SplitterStateGenre, arr);
}



LocalLibraryContainer::LocalLibraryContainer(QObject *parent) :
	LibraryContainerInterface(parent)
{

}

QString LocalLibraryContainer::get_name() const {
	return "local_library";
}

// LibraryViewInterface
QString LocalLibraryContainer::get_display_name() const {
	return tr("Local Library");
}

QIcon LocalLibraryContainer::get_icon() const {
	return Helper::get_icon("append");
}

QWidget* LocalLibraryContainer::get_ui() const {
	return static_cast<QWidget*>(_ui);
}

QComboBox* LocalLibraryContainer::get_libchooser(){
	return _ui->get_libchooser();
}

QMenu*LocalLibraryContainer::get_menu()
{
	if(_ui){
		return _ui->get_menu();
	}

	return nullptr;
}

void LocalLibraryContainer::init_ui()
{
	_ui = new GUI_LocalLibrary();
}

