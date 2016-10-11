/* CopyFolderThread.cpp */

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


#include "CopyThread.h"
#include "ImportCache.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/FileHelper.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Settings/Settings.h"

#include <QFile>
#include <QDir>

struct CopyThread::Private
{
	MetaDataList	v_md;
	QString			target_dir;
	QStringList		lst_copied_files;
	int				copied_files;

	int				percent;
	bool			cancelled;

	ImportCache		cache;
	CopyThread::Mode	mode;
};


CopyThread::CopyThread(const QString& target_dir, const ImportCache& cache, QObject *parent) :
	QThread(parent),
	SayonaraClass()
{
	_m = new CopyThread::Private();
	_m->cache = cache;
	_m->target_dir = target_dir;

	clear();
}

CopyThread::~CopyThread()
{
	delete _m; _m = nullptr;
}


void CopyThread::clear()
{
	_m->v_md.clear();
	_m->copied_files = 0;
	_m->lst_copied_files.clear();
	_m->mode = Mode::Copy;
	_m->percent = 0;
	_m->cancelled = false;
}


void CopyThread::emit_percent(int i, int n)
{
	int percent = (i * 100000) / n;
	_m->percent = percent / 1000;

	emit sig_progress(_m->percent);
}


void CopyThread::copy()
{
	clear();

	QStringList files = _m->cache.get_files();

	for(const QString& filename : files){

		if(_m->cancelled){
			return;
		}

		bool success;
		QString target_filename, target_dir;

		target_filename = _m->cache.get_target_filename(filename, _m->target_dir);
		if(target_filename.isEmpty()){
			continue;
		}

		target_dir = Helper::File::get_parent_directory(target_filename);

		success = Helper::File::create_directories(target_dir);
		if(!success){
			continue;
		}

		sp_log(Log::Debug) << "copy " << filename << " to \n\t" << target_filename;

		QFile f(filename);
		success = f.copy(target_filename);

		if(!success){
			continue;
		}

		MetaData md = _m->cache.get_metadata(filename);

		if(!md.filepath().isEmpty()){
			sp_log(Log::Debug) << "Set new filename: " << target_filename;
			md.set_filepath(target_filename);
			_m->v_md << md;
		}

		_m->lst_copied_files << target_filename;
		_m->copied_files++;

		emit_percent(_m->copied_files, files.size());
	}
}

void CopyThread::rollback()
{
	int n_operations = _m->lst_copied_files.size();
    int n_ops_todo = n_operations;
    int percent;

	QString lib_dir = _settings->get(Set::Lib_Path);
	QDir dir(lib_dir);

	for(const QString& f : _m->lst_copied_files) {
        QFile file(f);
        file.remove();
		percent = ((n_ops_todo--) * (_m->percent * 1000)) / (n_operations);

        emit sig_progress(percent/ 1000);
    }

	_m->percent = 0;
	_m->copied_files = 0;
	_m->lst_copied_files.clear();
}


void CopyThread::run()
{
	_m->cancelled = false;
	if(_m->mode == Mode::Copy){
		copy();
	}

	else if(_m->mode == Mode::Rollback){
		rollback();
	}
}


void CopyThread::cancel()
{
	_m->cancelled = true;
}

MetaDataList CopyThread::get_copied_metadata() const
{
	return _m->v_md;
}

bool CopyThread::was_cancelled() const
{
	return _m->cancelled;
}


int CopyThread::get_n_copied_files() const
{
	return _m->copied_files;
}


void CopyThread::set_mode(CopyThread::Mode mode)
{
	_m->mode = mode;
}

CopyThread::Mode CopyThread::get_mode() const
{
	return _m->mode;
}

