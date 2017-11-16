/* LibraryImporter.cpp */

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

#include "LibraryImporter.h"
#include "ImportCache.h"
#include "CachingThread.h"
#include "CopyThread.h"

#include "LocalLibrary.h"

#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Message/Message.h"
#include "Utils/Logger/Logger.h"

#include "Components/Tagging/ChangeNotifier.h"
#include "Database/DatabaseConnector.h"
#include "Database/LibraryDatabase.h"

#include <QMap>
#include <QDir>

struct LibraryImporter::Private
{
	LocalLibrary*			library=nullptr;
	CachingThread*			cache_thread=nullptr;
	CopyThread*				copy_thread=nullptr;
	ImportCachePtr			import_cache=nullptr;

	DB::Connector*			db=nullptr;

	LibraryImporter::ImportStatus status;
	QString					src_dir;

	Private(LocalLibrary* library) :
		library(library),
		db(DB::Connector::instance()),
		status(LibraryImporter::ImportStatus::NoTracks)
	{}
};

LibraryImporter::LibraryImporter(LocalLibrary* library) :
	QObject(library)
{
	m = Pimpl::make<Private>(library);

	Tagging::ChangeNotifier* md_change_notifier = Tagging::ChangeNotifier::instance();
	connect(md_change_notifier, &Tagging::ChangeNotifier::sig_metadata_changed,
			this, &LibraryImporter::metadata_changed);
}


LibraryImporter::~LibraryImporter() {}

void LibraryImporter::import_files(const QStringList& files, const QString& target_dir)
{
	emit_status(ImportStatus::Caching);

	if(!target_dir.isEmpty())
	{
		emit sig_target_dir_changed(target_dir);
	}

	CachingThread* thread = new CachingThread(files, m->library->library_path());
	connect(thread, &CachingThread::finished, this, &LibraryImporter::caching_thread_finished);
	connect(thread, &CachingThread::sig_progress, this, &LibraryImporter::sig_progress);
	connect(thread, &CachingThread::destroyed, [=]()
	{
		m->cache_thread = nullptr;
	});

	m->cache_thread = thread;
	thread->start();
}


// preload thread has cached everything, but ok button has not been clicked yet
void LibraryImporter::caching_thread_finished()
{
	CachingThread* thread = static_cast<CachingThread*>(sender());
	MetaDataList v_md;

	m->import_cache = thread->cache();
	if(!m->import_cache){
		emit_status(ImportStatus::NoTracks);
	}

	else {
		v_md = m->import_cache->get_soundfiles();
	}

	if(v_md.isEmpty() || thread->is_cancelled()){
		emit_status(ImportStatus::NoTracks);
	}

	else {
		emit_status(ImportStatus::Sleeping);
	}

	emit sig_got_metadata(v_md);

	thread->deleteLater();
}


// fired if ok was clicked in dialog
void  LibraryImporter::accept_import(const QString& target_dir)
{
	emit_status(ImportStatus::Importing);

	CopyThread* copy_thread = new CopyThread(target_dir, m->import_cache, this);

	connect(copy_thread, &CopyThread::sig_progress, this, &LibraryImporter::sig_progress);
	connect(copy_thread, &CopyThread::finished, this, &LibraryImporter::copy_thread_finished);
	connect(copy_thread, &CachingThread::destroyed, [=]()
	{
		m->copy_thread = nullptr;
	});

	m->copy_thread = copy_thread;
	copy_thread->start();
}


void LibraryImporter::copy_thread_finished()
{
	CopyThread* copy_thread = static_cast<CopyThread*>(sender());

	MetaDataList v_md = copy_thread->get_copied_metadata();

	emit_status(ImportStatus::Sleeping);

	// no tracks were copied or rollback was finished
	if(v_md.isEmpty()) {
		emit_status(ImportStatus::NoTracks);
		copy_thread->deleteLater();
		return;
	}

	// copy was cancelled
	sp_log(Log::Debug, this) << "Copy folder thread finished " << m->copy_thread->was_cancelled();
	if(copy_thread->was_cancelled()) {
		copy_thread->set_mode(CopyThread::Mode::Rollback);
		copy_thread->start();

		emit_status(ImportStatus::Rollback);
		return;
	}

	// store to db
	DB::Connector* db = DB::Connector::instance();
	DB::LibraryDatabase* lib_db = db->library_db(m->library->library_id(), db->db_id());

	bool success = lib_db->store_metadata(v_md);
	int n_files_copied = copy_thread->get_n_copied_files();
	int n_files_to_copy = m->import_cache->get_files().size();

	// error and success messages
	if(success)
	{
		m->db->clean_up();

		QString str = "";
		if(n_files_to_copy == n_files_copied) {
			str =  tr("All files could be imported");
		}

		else {
			str = tr("%1 of %2 files could be imported")
					.arg(n_files_copied)
					.arg(n_files_to_copy);
		}

		Message::info(str);

		emit_status(ImportStatus::Imported);

		Tagging::ChangeNotifier::instance()->change_metadata(MetaDataList(), MetaDataList());
	}

	else {
		QString warning = tr("Could not import tracks");
		Message::warning(warning);
	}

	copy_thread->deleteLater();
}


void LibraryImporter::metadata_changed(const MetaDataList& old_md, const MetaDataList& new_md)
{
	if(m->cache_thread){
		m->cache_thread->change_metadata(old_md, new_md);
	}
}


// fired if cancel button was clicked in dialog
void LibraryImporter::cancel_import()
{
	emit_status(ImportStatus::Cancelled);

	if(m->cache_thread && m->cache_thread->isRunning()){
		m->cache_thread->cancel();
	}

	else if(m->copy_thread && m->copy_thread->isRunning()){
		m->copy_thread->cancel();
	}
}

void LibraryImporter::emit_status(LibraryImporter::ImportStatus status)
{
	m->status = status;
	emit sig_status_changed(m->status);
}

LibraryImporter::ImportStatus LibraryImporter::get_status() const
{
	return m->status;
}
