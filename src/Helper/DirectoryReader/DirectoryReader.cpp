/* DirectoryReader.cpp */

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

#include "DirectoryReader.h"

#include "Helper/Tagging/Tagging.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Database/DatabaseConnector.h"

#include <QDirIterator>
#include <QFileInfo>



DirectoryReader::DirectoryReader () {

	_name_filters = Helper::get_soundfile_extensions();

}

DirectoryReader::~DirectoryReader () {

}

void DirectoryReader::set_filter(const QStringList & filter) {

	_name_filters = filter;
}

void DirectoryReader::set_filter(const QString& filter){
	_name_filters.clear();
	_name_filters << filter;
}


void DirectoryReader::get_files_in_dir_rec(QDir base_dir, QStringList& files) const
{
	QStringList tmp_files = base_dir.entryList(_name_filters,
					   (QDir::Filters)(QDir::Files | QDir::NoDotAndDotDot));

	QStringList dirs = base_dir.entryList((QDir::Filters)(QDir::Dirs | QDir::NoDotAndDotDot));

	for(const QString& dir : dirs){
		base_dir.cd(dir);
		get_files_in_dir_rec(base_dir, files);
		base_dir.cdUp();
	}

	for(const QString& filename : tmp_files){
		files << base_dir.absoluteFilePath(filename);
	}
}

QStringList DirectoryReader::get_files_in_dir(const QDir& base_dir) const
{

	QStringList entries, files;

	entries = base_dir.entryList(_name_filters, QDir::Files);

	for(const QString& file : entries) {
		files << base_dir.absoluteFilePath(file);
    }

	return files;
}

MetaDataList DirectoryReader::get_md_from_filelist(const QStringList& lst)
{

	MetaDataList v_md;
	QStringList files;
	QStringList playlist_paths;
	DatabaseConnector* db;

	// fetch sound and playlist files
	QStringList filter;
	filter << Helper::get_soundfile_extensions();
	filter << Helper::get_playlistfile_extensions();

	set_filter(filter);

	for( const QString& str : lst) {

		if(!QFile::exists(str)) {
			continue;
		}

		if(Helper::File::is_dir(str)) {

            QDir dir(str);
            dir.cd(str);

			get_files_in_dir_rec(dir, files);
        }

		else if(Helper::File::is_file(str)) {
			files << str;
        }
    }

	db = DatabaseConnector::getInstance();
	db->getMultipleTracksByPath(files, v_md);

	auto it = v_md.begin();
	while(it != v_md.end()){

		QString filepath = it->filepath();

		if(Helper::File::is_playlistfile(filepath)) {
			playlist_paths << filepath;
			it = v_md.erase(it);
            continue;
        }

		if(Helper::File::is_soundfile(filepath)) {

			if( it->id < 0 ) {
				if(!Tagging::getMetaDataOfFile(*it)) {
					it = v_md.erase(it);
                    continue;
                }

				it->is_extern = true;
            }
        }

		it++;
    }


    // TODO: look for playlists if paths could be read from database
	// extract media files out of playlist files
	for(const QString& path : playlist_paths) {

		sp_log(Log::Debug) << "parse playlist file " << path;

        MetaDataList v_md_pl;
        PlaylistParser::parse_playlist(path, v_md_pl);

        // check, that metadata is not already available
		for(const MetaData& md_pl : v_md_pl) {

            if(!v_md.contains(md_pl)) {
				v_md << std::move(md_pl);
            }
        }
	}

	return v_md;
}


QStringList DirectoryReader::find_files_rec(QDir dir, const QString& filename)
{
	if(dir.canonicalPath().isEmpty()){
		return QStringList();
	}

	if(filename.isEmpty()){
		return QStringList();
	}

	QStringList ret;
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList files = dir.entryList(QDir::Files);

	for(const QString& d : dirs){

		if(d.isEmpty()){
			continue;
		}

		QFileInfo fi(d);
		if(!fi.isDir()){
			continue;
		}

		dir.cd(d);
		ret += find_files_rec(dir, filename);
		dir.cdUp();
	}

	for(const QString& file : files){

		QFileInfo fi(file);
		if(!fi.isFile()){
			continue;
		}

		if(file.contains(filename)){
			ret += dir.absoluteFilePath(file);
		}
	}

	return ret;
}
