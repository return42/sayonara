/* GUI_LocalLibrary.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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
 *      Author: Lucio Carreras
 */

#include "GUI_LocalLibrary.h"
#include "GUI/Library/ui_GUI_LocalLibrary.h"
#include "GUI/Library/Helper/LocalLibraryMenu.h"
#include "GUI/Library/Models/DateSearchModel.h"
#include "GUI/Library/Views/AlbumCoverView.h"
#include "GUI/Library/Models/AlbumCoverModel.h"

#include "GUI/Helper/Library/LibraryDeleteDialog.h"
#include "GUI/Helper/SearchableWidget/SearchableView.h"

#include "Components/Library/LocalLibrary.h"
#include "InfoBox/GUI_LibraryInfoBox.h"
#include "ImportFolderDialog/GUI_ImportFolder.h"

#include "Helper/Helper.h"
#include "Helper/Library/DateFilter.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"
#include "Helper/Message/Message.h"
#include "Components/Covers/CoverLocation.h"

#include <QFileDialog>
#include <QDir>
#include <QTimer>
#include <QShortcut>
#include <QInputDialog>
#include <QMessageBox>


GUI_LocalLibrary::GUI_LocalLibrary(QWidget* parent) :
	GUI_AbstractLibrary(LocalLibrary::getInstance(), parent)
{
	setup_parent(this, &ui);

	LocalLibrary* library = LocalLibrary::getInstance();

	_local_library_menu = new LocalLibraryMenu(this);

	ui->pb_progress->setVisible(false);
	ui->lab_progress->setVisible(false);

	connect(_library, &LocalLibrary::sig_reloading_library, this, &GUI_LocalLibrary::progress_changed);
	connect(_library, &LocalLibrary::sig_reloading_library_finished, this, &GUI_LocalLibrary::reload_finished);
	connect(ui->btn_setLibrary, &QPushButton::clicked, this, &GUI_LocalLibrary::set_library_path_clicked);

	connect(ui->lv_album, &LibraryViewAlbum::sig_disc_pressed, this, &GUI_LocalLibrary::disc_pressed);
	connect(ui->lv_album, &LibraryViewAlbum::sig_import_files, this, &GUI_LocalLibrary::import_files);
	connect(ui->lv_album, &LibraryView::sig_merge, library, &LocalLibrary::merge_albums);

	connect(ui->lv_artist, &LibraryView::sig_import_files, this, &GUI_LocalLibrary::import_files);
	connect(ui->lv_artist, &LibraryView::sig_merge, library, &LocalLibrary::merge_artists);
	connect(ui->tb_title, &LibraryView::sig_import_files, this, &GUI_LocalLibrary::import_files);
	connect(ui->lv_genres, &QAbstractItemView::clicked, this, &GUI_LocalLibrary::genre_selection_changed);
	connect(ui->lv_genres, &QAbstractItemView::activated, this, &GUI_LocalLibrary::genre_selection_changed);
	connect(ui->lv_genres, &LibraryGenreView::sig_progress, this, &GUI_LocalLibrary::progress_changed);

	connect(ui->lv_date_search, &QAbstractItemView::clicked, this, &GUI_LocalLibrary::date_selection_changed);
	connect(ui->lv_date_search, &QAbstractItemView::activated, this, &GUI_LocalLibrary::date_selection_changed);

	connect(_local_library_menu, &LocalLibraryMenu::sig_reload_library, this, &GUI_LocalLibrary::reload_library_requested);
	connect(_local_library_menu, &LocalLibraryMenu::sig_import_file, this, &GUI_LocalLibrary::import_files_requested);
	connect(_local_library_menu, &LocalLibraryMenu::sig_import_folder, this, &GUI_LocalLibrary::import_dirs_requested);
	connect(_local_library_menu, &LocalLibraryMenu::sig_info, this, &GUI_LocalLibrary::show_info_box);
	connect(_local_library_menu, &LocalLibraryMenu::sig_libpath_clicked, this, &GUI_LocalLibrary::set_library_path_clicked);
	connect(_local_library_menu, &LocalLibraryMenu::sig_show_album_artists_changed, this, &GUI_LocalLibrary::refresh);
	connect(ui->btn_reload_library, &QPushButton::clicked, this, &GUI_LocalLibrary::reload_library_requested);

	connect(ui->splitter_artist_album, &QSplitter::splitterMoved, this, &GUI_LocalLibrary::splitter_artist_moved);
	connect(ui->splitter_tracks, &QSplitter::splitterMoved, this, &GUI_LocalLibrary::splitter_tracks_moved);
	connect(ui->splitter_genre, &QSplitter::splitterMoved, this, &GUI_LocalLibrary::splitter_genre_moved);
	connect(ui->splitter_date, &QSplitter::splitterMoved, this, &GUI_LocalLibrary::splitter_date_moved);

	connect(library, &LocalLibrary::sig_no_library_path, this, &GUI_LocalLibrary::lib_no_lib_path);
	connect(library, &LocalLibrary::sig_import_dialog_requested, this, &GUI_LocalLibrary::import_dialog_requested);

	new QShortcut(QKeySequence("Ctrl+."), this, SLOT(switch_album_view()));

	setAcceptDrops(true);

	QTimer::singleShot(0, library, SLOT(load()));

	if(ui->lv_genres->get_row_count() <= 1){
		ui->stacked_genre_widget->setCurrentIndex(1);
	}

	else{
		ui->stacked_genre_widget->setCurrentIndex(0);
	}

	REGISTER_LISTENER(Set::Lib_Path, _sl_libpath_changed);
}


GUI_LocalLibrary::~GUI_LocalLibrary()
{
	if(ui)
	{
		delete ui; ui = nullptr;
	}
}


QComboBox* GUI_LocalLibrary::get_libchooser() const
{
	return ui->combo_lib_chooser;
}

QMenu* GUI_LocalLibrary::get_menu() const
{
	return _local_library_menu;
}

void GUI_LocalLibrary::showEvent(QShowEvent* e)
{
	GUI_AbstractLibrary::showEvent(e);

	QByteArray artist_splitter_state, track_splitter_state, genre_splitter_state, date_splitter_state;

	artist_splitter_state = _settings->get(Set::Lib_SplitterStateArtist);
	track_splitter_state = _settings->get(Set::Lib_SplitterStateTrack);
	genre_splitter_state = _settings->get(Set::Lib_SplitterStateGenre);
	date_splitter_state = _settings->get(Set::Lib_SplitterStateDate);

	if(!artist_splitter_state.isEmpty()){
		ui->splitter_artist_album->restoreState(artist_splitter_state);
	}

	if(!track_splitter_state.isEmpty()){
		ui->splitter_tracks->restoreState(track_splitter_state);
	}

	if(!genre_splitter_state.isEmpty()){
		ui->splitter_genre->restoreState(genre_splitter_state);
	}

	if(!date_splitter_state.isEmpty()){
		ui->splitter_date->restoreState(date_splitter_state);
	}
}

void GUI_LocalLibrary::init_shortcuts()
{
	ui->le_search->setShortcutEnabled(QKeySequence::Find, true);

	new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(clear_button_pressed()), nullptr, Qt::WidgetWithChildrenShortcut);
	new QShortcut(QKeySequence::Find, ui->le_search, SLOT(setFocus()), nullptr, Qt::WindowShortcut);
}


Library::ReloadQuality GUI_LocalLibrary::show_quality_dialog()
{
	QStringList lst;
	bool ok = false;

	lst << tr("Check for changed files (fast)") + "\t";
	lst << tr("Deep scan (slow)") + "\t";

	QString str = QInputDialog::getItem(this,
						  "Sayonara",
						  tr("Select reload mode") + "\n",
						  lst,
						  0,
						  false,
						  &ok);

	if(!ok){
		return Library::ReloadQuality::Unknown;
	}

	if(str.isEmpty()){
		return Library::ReloadQuality::Unknown;
	}

	if(str.compare(lst.first()) == 0){
		return Library::ReloadQuality::Fast;
	}

	if(str.compare(lst[1]) == 0){
		return Library::ReloadQuality::Accurate;
	}

	return Library::ReloadQuality::Unknown;
}

void GUI_LocalLibrary::switch_album_view()
{
	int idx = (ui->stackedWidget->currentIndex() + 1) % 2;

	ui->stackedWidget->setCurrentIndex( idx );

	if(idx == 1 && ! _acv){
		init_album_cover_view();
		clear_button_pressed();
	}
}

void GUI_LocalLibrary::album_view_zoom_changed(int zoom)
{
	if(_acm){
		_acm->set_zoom(zoom);
	}
}


void GUI_LocalLibrary::language_changed()
{
	ui->retranslateUi(this);

	GUI_AbstractLibrary::language_changed();
}


void GUI_LocalLibrary::_sl_libpath_changed()
{
	QString library_path = _settings->get(Set::Lib_Path);
	if(!library_path.isEmpty()){
		ui->stacked_widget->setCurrentIndex(0);
	}

	else{
		ui->stacked_widget->setCurrentIndex(1);
	}

	ui->combo_searchfilter->setVisible(!library_path.isEmpty());
	ui->le_search->setVisible(!library_path.isEmpty());
	ui->btn_clear->setVisible(!library_path.isEmpty());
}

void GUI_LocalLibrary::clear_button_pressed()
{
	ui->lv_genres->clearSelection();
	ui->lv_date_search->clearSelection();

	GUI_AbstractLibrary::clear_button_pressed();
}

void GUI_LocalLibrary::genre_selection_changed(const QModelIndex& index)
{
	QVariant data = index.data();
	ui->combo_searchfilter->setCurrentIndex(1);
	ui->le_search->setText(data.toString());
	text_line_edited(data.toString());
}

void GUI_LocalLibrary::date_selection_changed(const QModelIndex& index)
{
	Library::Filter filter;
	Library::DateFilter date_filter = ui->lv_date_search->get_filter(index.row());
	filter.set_mode(Library::Filter::Date);
	filter.set_date_filter(date_filter);
	_library->psl_filter_changed(filter);
}


Library::TrackDeletionMode GUI_LocalLibrary::show_delete_dialog(int n_tracks)
{
	LibraryDeleteDialog dialog(n_tracks, this);
	dialog.exec();
	return dialog.answer();
}


void GUI_LocalLibrary::disc_pressed(int disc)
{
	LocalLibrary* ll = dynamic_cast<LocalLibrary*>(_library);
	ll->psl_disc_pressed(disc);
}


void GUI_LocalLibrary::lib_no_lib_path()
{
	Message::warning(tr("Please select your library path first and reload again."));

	QString dir = QFileDialog::getExistingDirectory(this, Lang::get(Lang::OpenDir),	QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(dir.size() > 3){
		_settings->set(Set::Lib_Path, dir);
	}
}

void GUI_LocalLibrary::progress_changed(const QString& type, int progress)
{
	ui->pb_progress->setVisible(progress >= 0);
	ui->lab_progress->setVisible(progress >= 0);

	ui->lab_progress->setText(type);

	if(progress == 0){
		if(ui->pb_progress->maximum() != 0){
			ui->pb_progress->setMaximum(0);
		}
	}

	if(progress > 0){
		if(ui->pb_progress->maximum() != 100){
			ui->pb_progress->setMaximum(100);
		}

		ui->pb_progress->setValue(progress);
	}
}


void GUI_LocalLibrary::reload_library_requested()
{
	Library::ReloadQuality quality = show_quality_dialog();
	if(quality == Library::ReloadQuality::Unknown){
		return;
	}

	_library->psl_reload_library(false, quality);
	ui->btn_reload_library->setEnabled(false);
}


void GUI_LocalLibrary::reload_finished()
{
	ui->btn_reload_library->setEnabled(true);
	ui->lv_genres->reload_genres();

	if(ui->lv_genres->get_row_count() <= 1){
		ui->stacked_genre_widget->setCurrentIndex(1);
	}
	else{
		ui->stacked_genre_widget->setCurrentIndex(0);
	}
}

void GUI_LocalLibrary::show_info_box()
{
	if(!_library_info_box){
		_library_info_box = new GUI_LibraryInfoBox(this);
	}

	_library_info_box->psl_refresh();
}

#include <QListView>
#include <QTreeView>
#include <QStandardPaths>
void GUI_LocalLibrary::import_dirs_requested()
{
	QStringList dirs;

	QFileDialog* dialog = new QFileDialog(this);
	dialog->setDirectory(QDir::homePath());
	dialog->setWindowTitle(Lang::get(Lang::ImportDir));
	dialog->setFileMode(QFileDialog::DirectoryOnly);
	dialog->setOption(QFileDialog::DontUseNativeDialog, true);
	QList<QUrl> sidebar_urls = dialog->sidebarUrls();

	QList<QStandardPaths::StandardLocation> locations;
	locations << QStandardPaths::HomeLocation;
	locations << QStandardPaths::DesktopLocation;
	locations << QStandardPaths::DownloadLocation;
	locations << QStandardPaths::MusicLocation;
	locations << QStandardPaths::TempLocation;

	for(QStandardPaths::StandardLocation location : locations)
	{
		QStringList std_locations = QStandardPaths::standardLocations(location);
		for(const QString& std_location : std_locations){
			QUrl url = QUrl::fromLocalFile(std_location);
			if(sidebar_urls.contains(url)){
				continue;
			}

			sidebar_urls << url;
		}
	}

	dialog->setSidebarUrls(sidebar_urls);

	QListView* list_view = dialog->findChild<QListView*>("listView");
	if(list_view == nullptr)
	{
		delete dialog;

		QString dir = QFileDialog::getExistingDirectory(this, Lang::get(Lang::ImportDir),
					QDir::homePath(), QFileDialog::ShowDirsOnly);
		if(!dir.isEmpty()){
			dirs << dir;
		}
	}

	else{
		list_view->setSelectionMode(QAbstractItemView::MultiSelection);
		QTreeView* tree_view = dialog->findChild<QTreeView*>();
		if(tree_view){
			tree_view->setSelectionMode(QAbstractItemView::MultiSelection);
		}

		if(dialog->exec() == QFileDialog::Accepted){
			dirs = dialog->selectedFiles();
		}
	}

	if(!dirs.isEmpty()){
		_library->import_files(dirs);
	}
}

void GUI_LocalLibrary::import_files_requested()
{
	QStringList extensions = Helper::get_soundfile_extensions();
	QString filter = QString("Soundfiles (") + extensions.join(" ") + ")";
	QStringList files = QFileDialog::getOpenFileNames(this, Lang::get(Lang::ImportFiles),
					QDir::homePath(), filter);

	if(files.size() > 0) {
		_library->import_files(files);
	}
}


void GUI_LocalLibrary::import_files(const QStringList& files)
{
	_library->import_files(files);
}


void GUI_LocalLibrary::import_dialog_requested()
{
	if(!_ui_importer){
		_ui_importer = new GUI_ImportFolder(this, true);
	}

	_ui_importer->show();
}


void GUI_LocalLibrary::set_library_path_clicked()
{
	QString start_dir = QDir::homePath();
	QString old_dir = _settings->get(Set::Lib_Path);

	if (old_dir.size() > 0 && QFile::exists(old_dir)) {
		start_dir = old_dir;
	}

	QString dir = QFileDialog::getExistingDirectory(this, Lang::get(Lang::OpenDir),
			old_dir, QFileDialog::ShowDirsOnly);

	if(dir.isEmpty()){
		return;
	}

	if(old_dir.compare(dir) == 0) {
		return;
	}

	_settings->set(Set::Lib_Path, dir);

	GlobalMessage::Answer answer = Message::question_yn(tr("Do you want to reload the Library?"), "Library");

	if(answer == GlobalMessage::Answer::No){
		return;
	}

	Library::ReloadQuality quality = show_quality_dialog();
	if(quality == Library::ReloadQuality::Unknown){
		return;
	}

	_library->psl_reload_library(false, quality);
}


void GUI_LocalLibrary::splitter_artist_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	QByteArray arr = ui->splitter_artist_album->saveState();
	_settings->set(Set::Lib_SplitterStateArtist, arr);
}

void GUI_LocalLibrary::splitter_tracks_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	QByteArray arr = ui->splitter_tracks->saveState();
	_settings->set(Set::Lib_SplitterStateTrack, arr);
}

void GUI_LocalLibrary::splitter_genre_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	QByteArray arr = ui->splitter_genre->saveState();
	_settings->set(Set::Lib_SplitterStateGenre, arr);
}

void GUI_LocalLibrary::splitter_date_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	QByteArray arr = ui->splitter_date->saveState();
	_settings->set(Set::Lib_SplitterStateDate, arr);
}



void GUI_LocalLibrary::init_album_cover_view()
{
	_acv = new AlbumCoverView(ui->page_4);
	ui->page_4->layout()->addWidget(_acv);

	_acm = new AlbumCoverModel(_acv);

	_acv->setModel(_acm);
	_acv->show();

	connect(_acv, &LibraryView::doubleClicked, this, &GUI_LocalLibrary::album_dbl_clicked);
	connect(_acv, &LibraryView::sig_sel_changed, this, &GUI_LocalLibrary::album_sel_changed);
	connect(_acv, &LibraryView::sig_middle_button_clicked, this, &GUI_LocalLibrary::album_middle_clicked);
	connect(_acv, &AlbumCoverView::sig_zoom_changed, _acm, &AlbumCoverModel::set_zoom);
}


#include "Helper/Logger/Logger.h"
void GUI_LocalLibrary::lib_fill_albums(const AlbumList& albums)
{
	GUI_AbstractLibrary::lib_fill_albums(albums);
	if(!_acv){
		return;
	}

	QList<CoverLocation> covers;

	for(const Album& album : albums){
		// TODO: Maybe we should try some Pool of covers
		// TODO: Do not look inside the CoverLocation class every time
		// TODO: Make covers downloadable
		// TODO: Adjust size of that thing
		CoverLocation cl = CoverLocation::get_cover_location(album);
		covers << cl;
	}

	_acm->set_data(albums, covers);
	_acv->resizeRowsToContents();
	_acv->resizeColumnsToContents();
}


void GUI_LocalLibrary::lib_fill_tracks(const MetaDataList& v_md)
{
	GUI_AbstractLibrary::lib_fill_tracks(v_md);

	if(_acm){
		_acm->set_mimedata(v_md);
	}
}

