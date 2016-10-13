/* IndexDirectoriesThread.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#include "IndexDirectoriesThread.h"
#include "Helper/Set.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaDataList.h"

struct IndexDirectoriesThread::Private
{
	MetaDataList	v_md;
	QStringList		directories;
};

IndexDirectoriesThread::IndexDirectoriesThread(const MetaDataList& v_md)
{
	_m = Pimpl::make<IndexDirectoriesThread::Private>();
	_m->v_md = v_md;
}

IndexDirectoriesThread::~IndexDirectoriesThread() {}

QStringList IndexDirectoriesThread::get_directories() const
{
	return _m->directories;
}

void IndexDirectoriesThread::run()
{
	_m->directories.clear();

	SP::Set<QString> paths;
	for(const MetaData& md : _m->v_md)
	{
		paths.insert(Helper::File::get_parent_directory(md.filepath()));
	}

	for(auto it=paths.begin(); it!=paths.end(); it++){
		_m->directories << *it;
	}
}

