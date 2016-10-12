/* ImportCachingThread.cpp */

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



#include "CachingThread.h"
#include "ImportCache.h"

#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaDataList.h"

struct CachingThread::Private {
	ImportCache		cache;
	QStringList		file_list;
	bool			cancelled;


	void read_files()
	{
		DirectoryReader dr;
		dr.set_filter("*");

		for(const QString& filename : file_list){

			if(cancelled){
				cache.clear();
				return;
			}

			if(Helper::File::is_dir(filename)){
				QStringList dir_files;
				QDir dir(filename);

				dr.get_files_in_dir_rec(dir, dir_files);

				for(const QString& dir_file : dir_files){
					cache.add_standard_file(dir_file, filename);
				}
			}

			else{
				cache.add_standard_file(filename);
			}
		}
	}

	void extract_soundfiles()
	{

		for(const QString& filename : cache.get_files()){

			if(Helper::File::is_soundfile(filename)){

				MetaData md;
				md.set_filepath(filename);

				bool success = Tagging::getMetaDataOfFile(md);
				if(success){
					cache.add_soundfile(md);
				}
			}
		}
	}
};

CachingThread::CachingThread(const QStringList& file_list, QObject *parent) :
	QThread(parent)
{
	_m = Pimpl::make<CachingThread::Private>();

	_m->file_list = file_list;
	_m->cancelled = false;
}

CachingThread::~CachingThread()
{

}

void CachingThread::run()
{
	_m->cache.clear();

	_m->read_files();
	_m->extract_soundfiles();

	emit sig_progress( -1 );
}


void CachingThread::change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new){
	_m->cache.change_metadata(v_md_old, v_md_new);
}


ImportCache CachingThread::get_cache() const
{
	return _m->cache;
}

void CachingThread::cancel()
{
	_m->cancelled = true;
}

bool CachingThread::is_cancelled() const
{
	return _m->cancelled;
}

