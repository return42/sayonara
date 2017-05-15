/* FileSystemWatcher.cpp */

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

#include "FileSystemWatcher.h"
#include "Helper/FileHelper.h"
#include "Helper/Helper.h"
#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/Logger/Logger.h"

#include <algorithm>

const int SleepingTimeSec = 15;

struct FileSystemWatcher::Private
{
	QStringList	indexed_files;
	QString	library_path;
	bool may_run;
	bool refresh;
	bool waiting;

	Private(const QString& library_path) :
		library_path(library_path),
		may_run(true),
		refresh(false),
		waiting(false)
	{}

};

FileSystemWatcher::FileSystemWatcher(const QString& library_path, QObject* parent) :
	QThread(parent)
{
	_m = Pimpl::make<Private>(library_path);
}

FileSystemWatcher::~FileSystemWatcher() {}

void FileSystemWatcher::run()
{
	_m->may_run = true;
	_m->refresh = false;
	_m->waiting = false;

	_m->indexed_files = index_files(_m->library_path);

	while(_m->may_run)
	{
		bool changed = false;

		if(_m->refresh)
		{
			if(!_m->may_run){
				break;
			}

			_m->indexed_files = index_files(_m->library_path);
			_m->refresh = false;
			_m->waiting = false;
		}

		if(!_m->waiting)
		{
			if(!_m->may_run){
				break;
			}

			QStringList actual_files = index_files(_m->library_path);

			if(actual_files.size() != _m->indexed_files.size()){
				changed = true;
			}

			else
			{
				auto it1 = _m->indexed_files.begin();
				auto it2 = actual_files.begin();

				for(; it1 != _m->indexed_files.end(); it1++, it2++){
					if(!_m->may_run){
						break;
					}

					if(it1->compare(*it2) != 0){
						changed = true;
						break;
					}
				}
			}
		}

		if(changed && _m->may_run){
			_m->waiting = true;
			emit sig_changed();
		}

		for(int i=0; i<SleepingTimeSec; i++){
			if(!_m->may_run){
				break;
			}

			QThread::msleep(1000);
		}

		if(!_m->may_run){
			break;
		}
	}

	sp_log(Log::Info) << "Watcher finished";
}

QStringList FileSystemWatcher::index_files(const QString& root)
{
	DirectoryReader reader;
	QStringList filters;
	filters << Helper::get_soundfile_extensions();

	QStringList files;
	reader.get_files_in_dir_rec(QDir(root), files);

	if(!_m->may_run){
		return QStringList();
	}

	std::sort(files.begin(), files.end());

	return files;
}

void FileSystemWatcher::refresh()
{
	_m->refresh = true;
}

void FileSystemWatcher::stop()
{
	_m->may_run = false;
}
