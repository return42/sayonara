/* ReloadThread.cpp */

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
 * ReloadThread.cpp
 *
 *  Created on: Jun 19, 2011
 *      Author: Lucio Carreras
 */

#define N_FILES_TO_STORE 500

#include "ReloadThread.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/MetaData/Genre.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Language.h"
#include "Database/DatabaseConnector.h"

#include <utility>

struct ReloadThread::Private
{
	DatabaseConnector*		db=nullptr;
	QString					library_path;
	MetaDataList			v_md;
	Library::ReloadQuality	quality;
	bool					paused;
	bool					running;
};

ReloadThread::ReloadThread(QObject *parent) :
	QThread(parent),
	SayonaraClass()
{
	_m = Pimpl::make<ReloadThread::Private>();
	_m->db = DatabaseConnector::getInstance();

	_m->paused = false;
	_m->running = false;

	_m->library_path = _settings->get(Set::Lib_Path);
	_m->quality = Library::ReloadQuality::Fast;
}

ReloadThread::~ReloadThread() {}

bool ReloadThread::compare_md(const MetaData& md1, const MetaData& md2)
{
	if(md1.genres.count() != md2.genres.count()){
		return false;
	}

	auto it1 = md1.genres.begin();
	auto it2 = md2.genres.begin();
	for(int i=0; i<md1.genres.count(); i++, it1++, it2++){
		if(!it1->is_equal(*it2)){
			return false;
		}
	}

	return (md1.title == md2.title &&
			md1.album == md2.album &&
			md1.artist == md2.artist &&
			md1.year == md2.year &&
			md1.rating == md2.rating &&
			md1.discnumber == md2.discnumber &&
			md1.track_num == md2.track_num &&
			md1.album_artist() == md2.album_artist() &&
			md1.album_artist_id() == md2.album_artist_id()
	);
}

int ReloadThread::get_and_save_all_files(const QHash<QString, MetaData>& md_map_lib) 
{
	QString library_path = _m->library_path;

	if(library_path.isEmpty() || !QFile::exists(library_path)) {
		return 0;
	}

	DatabaseConnector* db = _m->db;
	QDir dir(library_path);

	MetaDataList v_md_to_store;
	QStringList files = get_files_recursive (dir);

	int n_files = files.size();
	int cur_idx_files=0;

	for(const QString& filepath : files){
		bool file_was_read = false;
		MetaData md(filepath);
		const MetaData& md_lib = md_map_lib[filepath];

		int percent = (cur_idx_files++ * 100) / n_files;
		emit sig_reloading_library(Lang::get(Lang::ReloadLibrary).triplePt(), percent);

		if(md_lib.id >= 0){
			if(_m->quality == Library::ReloadQuality::Fast){
				continue;
			}

			file_was_read = Tagging::getMetaDataOfFile(md, Tagging::Quality::Dirty);

			if(!file_was_read){		
				continue;
			}

			if( md_lib.length_ms > 1000 && md_lib.length_ms < 3600000 && compare_md(md, md_lib)){
				continue;
			}
		}

		file_was_read = Tagging::getMetaDataOfFile(md, Tagging::Quality::Quality);

		if(file_was_read){
			v_md_to_store << md;

			if(v_md_to_store.size() >= N_FILES_TO_STORE){
				db->storeMetadata(v_md_to_store);
				v_md_to_store.clear();
			}
		}
	}

	if(!v_md_to_store.isEmpty()){
		db->storeMetadata(v_md_to_store);
		v_md_to_store.clear();
	}

	db->addAlbumArtists();
	db->createIndexes();
	db->clean_up();

	return v_md_to_store.size();
}


QStringList ReloadThread::get_files_recursive(QDir base_dir) 
{
	QStringList ret;
	QString message = tr("Reading files from file system") + "... ";
	emit sig_reloading_library(message, 0);

	QStringList soundfile_exts = Helper::get_soundfile_extensions();
	QStringList sub_dirs;
	QStringList sub_files;

	sub_dirs = base_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for(const QString& dir : sub_dirs) {
		bool success = base_dir.cd(dir);

		if(!success){
			continue;
		}

		ret << get_files_recursive(base_dir);

		base_dir.cdUp();
    }

	sub_files = base_dir.entryList(soundfile_exts, QDir::Files);
	if(sub_files.isEmpty()){
		return ret;
	}

	ret << process_sub_files(base_dir, sub_files);

	return ret;
}


QStringList ReloadThread::process_sub_files(const QDir& base_dir, const QStringList& sub_files)
{
	QStringList lst;
	for(const QString& filename : sub_files) {
		QString abs_path = base_dir.absoluteFilePath(filename);
		QFileInfo info(abs_path);

		if(!info.exists()){
			sp_log(Log::Warning) << "File " << abs_path << " does not exist. Skipping...";
			continue;
		}

		if(!info.isFile()){
			sp_log(Log::Warning) << "Error: File " << abs_path << " is not a file. Skipping...";
			continue;
		}

		lst << abs_path;
	}

	return lst;
}


void ReloadThread::pause() 
{
    _m->paused = true;
}

void ReloadThread::goon() 
{
	_m->paused = false;
}

bool ReloadThread::is_running() const
{
	return _m->running;
}

void ReloadThread::set_quality(Library::ReloadQuality quality)
{
	_m->quality = quality;
}

void ReloadThread::run() 
{
	if(_m->running){
		return;
	}

	DatabaseConnector* db = _m->db;

	_m->running = true;
    _m->paused = false;

	MetaDataList v_md, v_to_delete;
	QHash<QString, MetaData> v_md_map;

	emit sig_reloading_library(tr("Delete orphaned tracks..."), 0);

	db->deleteInvalidTracks();
	db->getAllTracks(v_md);

	sp_log(Log::Debug) << "Have " << v_md.size() << " tracks";

	// find orphaned tracks in library && delete them
	for(const MetaData& md : v_md){
		if(!Helper::File::check_file(md.filepath())) {
			v_to_delete << std::move(md);
		}

		else{
			v_md_map[md.filepath()] = md;
		}
	}

	if(!v_to_delete.isEmpty()){
		db->deleteTracks(v_to_delete);
	}

	get_and_save_all_files(v_md_map);

    _m->paused = false;
	_m->running = false;
}

void ReloadThread::set_lib_path(const QString& library_path) 
{
	_m->library_path = library_path;
}

