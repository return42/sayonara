/* ReloadThread.cpp */

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
 * ReloadThread.cpp
 *
 *  Created on: Jun 19, 2011
 *      Author: luke
 */

#define N_FILES_TO_STORE 500

#include "ReloadThread.h"
#include "Helper/Helper.h"
#include "Database/DatabaseConnector.h"
#include "Helper/DirectoryReader/DirectoryReader.h"

ReloadThread::ReloadThread(QObject *parent) :
	QThread(parent),
	SayonaraClass()
{

	_db = DatabaseConnector::getInstance();

	_paused = false;
	_running = false;

	_library_path = _settings->get(Set::Lib_Path);
}


ReloadThread::~ReloadThread() {

}


int ReloadThread::get_and_save_all_files(const QMap<QString, MetaData>& md_map_lib) {

	if(_library_path.size() == 0 || !QFile::exists(_library_path)) {
		return 0;
	}

	QDir dir(_library_path);
	MetaDataList v_md, v_md_to_store;

	get_files_recursive (dir, v_md, &n_files);
	int cur_idx_files=0;
	int n_files = v_md.size();

	for(const MetaData& md : v_md){
		QString filepath = md.filepath();
		MetaData md_lib = md_map_lib[filepath];
		cur_idx_files++;

		emit sig_reloading_library(tr("Reloading library"), (cur_idx_files * 100 / n_files));

		if(md_lib.id >= 0){
			continue;
		}

		sp_log(Log::Debug) << "Could not find: " << filepath;

		MetaData md_full;
		md_full.set_filepath(filepath);

		if(Tagging::getMetaDataOfFile(md_full, Tagging::Quality::Standard)){
			sp_log(Log::Debug) << "Insert metadata " << md.filepath() << ": " << md.bitrate;
			v_md_to_store << md_full;
		}

		if(v_md_to_store.size() >= N_FILES_TO_STORE){
			_db->storeMetadata(v_md_to_store);
			v_md_to_store.clear();
		}
	}

	_db->createIndexes();
	_db->clean_up();

	return v_md_to_store.size();
}


void ReloadThread::get_files_recursive(QDir base_dir, MetaDataList& v_md, int* n_files) {

	int num_files;
	QStringList soundfile_exts = Helper::get_soundfile_extensions();
	QStringList sub_dirs;
	QStringList sub_files;


	sub_dirs = base_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for(const QString& dir : sub_dirs) {

		bool success;
		QFileInfo info(base_dir.absoluteFilePath(dir));

		if(!info.exists()){
			sp_log(Log::Warning) << "File " << dir << " does not exist. Skipping...";
			continue;
		}

		if(!info.isDir()){
			sp_log(Log::Warning) << "File " << dir << " is not a directory. Skipping...";
			continue;
		}

		success = base_dir.cd(dir);

		if(!success){
			continue;
		}

		get_files_recursive(base_dir, v_md, n_files);
		base_dir.cdUp();
    }

	num_files = *n_files;

	sub_files = base_dir.entryList(soundfile_exts, QDir::Files);

	v_md << process_sub_files(base_dir, sub_files);

    *n_files = num_files;
}


MetaDataList process_sub_files(const QDir& base_dir, const QStringList& sub_files){

	MetaDataList v_md;
	for(const QString& filename : sub_files) {

		MetaData md;
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

		md.set_filepath( abs_path );

		if( Tagging::getMetaDataOfFile(md, Tagging::Quality::Dirty)) {
			v_md << std::move(md);
		}
	}

	return v_md;
}



void ReloadThread::pause() {
    _paused = true;
}

void ReloadThread::goon() {
	_paused = false;
}

bool ReloadThread::is_running() const
{
	return _running;
}

void ReloadThread::run() {

	if(_running){
		return;
	}

	_running = true;
    _paused = false;

	MetaDataList v_md, v_to_delete;
	QMap<QString, MetaData> v_md_map;

	emit sig_reloading_library(tr("Delete orphaned tracks..."), 0);

	_db->deleteInvalidTracks();
	_db->getTracksFromDatabase(v_md);

	sp_log(Log::Debug) << "Have " << v_md.size() << " tracks";

	// find orphaned tracks in library && delete them
	for(const MetaData& md : v_md){

		if(!Helper::check_track(md)) {
			v_to_delete << std::move(md);
		}

		else{
			v_md_map[md.filepath()] = md;
		}
	}

	if(!v_to_delete.isEmpty()){
		_db->deleteTracks(v_to_delete);
	}

	get_and_save_all_files(v_md_map);

    _paused = false;
	_running = false;

}


void ReloadThread::set_lib_path(const QString& library_path) {
	_library_path = library_path;
}



