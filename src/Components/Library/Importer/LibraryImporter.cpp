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

#include "Helper/Settings/Settings.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Message/Message.h"
#include "Helper/Logger/Logger.h"

#include "Components/TagEdit/MetaDataChangeNotifier.h"
#include "Database/DatabaseConnector.h"
#include "CachingThread.h"
#include "CopyThread.h"

#include <QMap>
#include <QDir>

struct LibraryImporter::Private
{
	CachingThread*			cache_thread=nullptr;
	CopyThread*				copy_thread=nullptr;

	ImportCache				import_cache;

	DatabaseConnector*		db=nullptr;

	LibraryImporter::ImportStatus
							status;

	QString					src_dir;

	Private()
	{
		db = DatabaseConnector::getInstance();
		status = LibraryImporter::ImportStatus::NoTracks;
		cache_thread = nullptr;
		copy_thread = nullptr;
	}
};

LibraryImporter::LibraryImporter(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{
	_m = Pimpl::make<Private>();

	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_changed,
			this, &LibraryImporter::metadata_changed);
}

LibraryImporter::~LibraryImporter() {}

void LibraryImporter::import_files(const QStringList& files)
{
	emit_status(ImportStatus::Caching);

	CachingThread* thread = new CachingThread(files);

	connect(thread, &CachingThread::finished, this, &LibraryImporter::caching_thread_finished);
	connect(thread, &CachingThread::sig_progress, this, &LibraryImporter::sig_progress);
	connect(thread, &CachingThread::destroyed, [=]()
	{
		_m->cache_thread = nullptr;
	});

	_m->cache_thread = thread;
	thread->start();
}


// preload thread has cached everything, but ok button has not been clicked yet
void LibraryImporter::caching_thread_finished()
{
	CachingThread* thread = static_cast<CachingThread*>(sender());
	MetaDataList v_md ;

	_m->import_cache = thread->get_cache();

	v_md = _m->import_cache.get_soundfiles();

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

	CopyThread* copy_thread = new CopyThread(target_dir, _m->import_cache, this);

	connect(copy_thread, &CopyThread::sig_progress, this, &LibraryImporter::sig_progress);
	connect(copy_thread, &CopyThread::finished, this, &LibraryImporter::copy_thread_finished);
	connect(copy_thread, &CachingThread::destroyed, [=]()
	{
		_m->copy_thread = nullptr;
	});

	_m->copy_thread = copy_thread;
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
	sp_log(Log::Debug, this) << "Copy folder thread finished " << _m->copy_thread->was_cancelled();
	if(copy_thread->was_cancelled()) {
		copy_thread->set_mode(CopyThread::Mode::Rollback);
		copy_thread->start();

		emit_status(ImportStatus::Rollback);
		return;
	}

	// store to db
	bool success = _m->db->storeMetadata(v_md);
	int n_files_copied = copy_thread->get_n_copied_files();
	int n_files_to_copy = _m->import_cache.get_files().size();

	// error and success messages
	if(success) {
		_m->db->clean_up();

		QString str = "";
		if(n_files_to_copy == n_files_copied){
			str =  tr("All files could be imported");
		}

		else{
			str = tr("%1 of %2 files could be imported")
					.arg(n_files_copied)
					.arg(n_files_to_copy);
		}

		Message::info(str);

		emit_status(ImportStatus::Imported);

		MetaDataChangeNotifier::getInstance()->change_metadata(MetaDataList(), MetaDataList());
	}

	else{
		QString warning = tr("Sorry, but tracks could not be imported") +
					"<br />" +
					tr("Please use the import function of the file menu<br /> or move tracks to library and use 'Reload library'");

		Message::warning(warning);
	}

	copy_thread->deleteLater();
}


void LibraryImporter::metadata_changed(const MetaDataList& old_md, const MetaDataList& new_md)
{
	if(_m->cache_thread){
		_m->cache_thread->change_metadata(old_md, new_md);
	}

	_m->import_cache.change_metadata(old_md, new_md);
}


// fired if cancel button was clicked in dialog
void LibraryImporter::cancel_import()
{
	emit_status(ImportStatus::Cancelled);

	if(_m->cache_thread && _m->cache_thread->isRunning()){
		_m->cache_thread->cancel();
	}

	else if(_m->copy_thread && _m->copy_thread->isRunning()){
		_m->copy_thread->cancel();
	}
}

void LibraryImporter::emit_status(LibraryImporter::ImportStatus status)
{
	_m->status = status;
	emit sig_status_changed(_m->status);
}

LibraryImporter::ImportStatus LibraryImporter::get_status() const
{
	return _m->status;
}
