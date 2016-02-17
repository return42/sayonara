/* LibraryImporter.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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

#include "LibraryImporter.h"

#include "Components/TagEdit/MetaDataChangeNotifier.h"
#include "Database/DatabaseConnector.h"
#include "Helper/MetaData/MetaData.h"
#include "threads/ImportCachingThread.h"
#include "threads/ImportCopyThread.h"

#include "GUI/Helper/GlobalMessage/Message.h"

#include <QMap>
#include <QDir>



LibraryImporter::LibraryImporter(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{


	_db = DatabaseConnector::getInstance();

	_caching_thread = new ImportCachingThread(this);
	_copy_thread = new ImportCopyThread(this);

	_lib_path = _settings->get(Set::Lib_Path);
	_status = ImportStatus::NoTracks;

	connect(_caching_thread, &ImportCachingThread::finished, this, &LibraryImporter::caching_thread_finished);
	connect(_caching_thread, &ImportCachingThread::sig_done, this, &LibraryImporter::caching_thread_done);
	connect(_caching_thread, &ImportCachingThread::sig_progress, this, &LibraryImporter::sig_progress);

	connect(_copy_thread, &ImportCopyThread::finished, this, &LibraryImporter::copy_thread_finished);
	connect(_copy_thread, &ImportCopyThread::sig_progress, this, &LibraryImporter::sig_progress);

	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_changed,
			this, &LibraryImporter::metadata_changed);
}

LibraryImporter::~LibraryImporter(){

}


void LibraryImporter::import_dir(const QString& dir) {

	QStringList lst;
	lst << dir;
	import_files(lst);
}


void LibraryImporter::import_files(const QStringList& list) {

	QDir lib_dir(_lib_path);
	QStringList lib_dirs = lib_dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);
	lib_dirs.push_front("");

	emit sig_got_library_dirs(lib_dirs);
	emit_status(ImportStatus::Caching);

	_caching_thread->set_filelist(list);
	_caching_thread->start();
}


// preload thread has cached everything, but ok button has not been clicked yet
void LibraryImporter::caching_thread_done() {

	MetaDataList v_md = _caching_thread->get_metadata();

	if(v_md.isEmpty()){
		emit_status(ImportStatus::NoTracks);
	}

	else {
		emit_status(ImportStatus::Sleeping);
	}

	emit sig_got_metadata(v_md);
}


// Caching is done, ok has been clicked
void LibraryImporter::caching_thread_finished() {

	emit sig_progress(0);
	if(_caching_thread->get_n_tracks() == 0){
		emit_status(ImportStatus::NoTracks);
		return;
	}

	if(!_caching_thread->was_cancelled())
	{
		start_copy_thread();
	}
}


void LibraryImporter::start_copy_thread(){

	QStringList files = _caching_thread->get_extracted_files();
	QMap<QString, MetaData> md_map = _caching_thread->get_md_map();
	QMap<QString, QString> pd_map = _caching_thread->get_pd_map();

	_copy_thread->set_vars(_import_to, files, md_map, pd_map);
	_copy_thread->set_mode(ImportCopyThread::Mode::Copy);
	_copy_thread->start();

	emit_status(ImportStatus::Importing);
}



void LibraryImporter::copy_thread_finished() {

	MetaDataList v_md = _copy_thread->get_metadata();

	emit_status(ImportStatus::Sleeping);

	// no tracks were copied or rollback was finished
	if(v_md.size() == 0) {
		emit_status(ImportStatus::NoTracks);
		return;
	}

	// copy was cancelled
	sp_log(Log::Debug) << "Copy folder thread finished " << _copy_thread->get_cancelled();
	if(_copy_thread->get_cancelled()) {

		_copy_thread->set_mode(ImportCopyThread::Mode::Rollback);
		_copy_thread->start();

		emit_status(ImportStatus::Rollback);

		return;
	}

	// store to db
	bool success = _db->storeMetadata(v_md);
	int n_snd_files = _copy_thread->get_n_files();
	int n_files_copied = _copy_thread->get_copied_files();

	// error and success messages
	if(success) {

		_db->clean_up();

		QString str = "";
		if(n_snd_files == n_files_copied){
			str =  tr("All files could be imported");
		}

		else{
			str = tr("%1 of %2 files could be imported").arg(n_files_copied).arg(n_snd_files);
		}

		Message::info(str);

		emit_status(ImportStatus::Imported);

		MetaDataChangeNotifier::getInstance()->change_metadata(MetaDataList(), MetaDataList());
	}

	else{
		Message::warning(
					tr("Sorry, but tracks could not be imported") +
					"<br />" +
					tr("Please use the import function of the file menu<br /> or move tracks to library and use 'Reload library'")
					);
	}
}


void LibraryImporter::metadata_changed(const MetaDataList& old_md, const MetaDataList& new_md){

	_caching_thread->update_metadata(old_md, new_md);
}


// fired if ok was clicked in dialog
void  LibraryImporter::accept_import(const QString& target_dir) {

	// the preload thread may terminate now
	_caching_thread->set_may_terminate();
	_import_to = target_dir;
}


// fired if cancel button was clicked in dialog
void LibraryImporter::cancel_import() {

	emit_status(ImportStatus::Cancelled);

	// preload thread
	if(_caching_thread->isRunning()) {
		_caching_thread->set_cancelled();
	}

	// copy folder thread
	else if(_copy_thread->isRunning()) {

		// useless during rollback
		if(_copy_thread->get_mode() == ImportCopyThread::Mode::Rollback) {
			return;
		}

		_copy_thread->set_cancelled();
	}
}

void LibraryImporter::emit_status(LibraryImporter::ImportStatus status)
{
	_status = status;
	emit sig_status_changed(_status);
}

LibraryImporter::ImportStatus LibraryImporter::get_status() const{
	return _status;
}
