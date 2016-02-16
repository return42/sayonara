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



#include "threads/ImportCopyThread.h"
#include "Helper/Helper.h"

#include <QDir>
#include <QFile>

ImportCopyThread::ImportCopyThread(QObject *parent) :
	QThread(parent),
	SayonaraClass()
{
	_n_files = 0;
	_copied_files = 0;
	_mode = Mode::Copy;
	_percent = 0;
	_cancelled = false;
}



void ImportCopyThread::emit_percent(int i, int n) {

	int percent = (i * 100000) / n;
	_percent = percent / 1000;

	emit sig_progress(_percent);
}


QString _create_target_path( const QString& src_dir,    // dir chosen when "import dir"
							 const QString& filename,   // filename of current file
							 const QString& lib_dir,    // library path
							 const QString& chosen_dir) // chosen dir typed in the import dialog
{
	QString subfolders;

	QString parent = Helper::File::get_parent_directory(src_dir);
	QString pure_src_dir = src_dir;
	pure_src_dir.remove(parent);


	if(src_dir.size() > 0) {

		QString pure_filename;
		// extract folders between the files and src dir and create directories
		// /home/user/dir/subfolder/subfolder2/bla.mp3 -> subfolder/subfolder2
		Helper::File::split_filename(filename, subfolders, pure_filename);

		subfolders.remove(src_dir);
	}

	QString target_path =
			lib_dir + QDir::separator() +
			chosen_dir + QDir::separator() +
			pure_src_dir + QDir::separator() +
			subfolders;

	if(!QFile::exists(target_path)) {
		QDir::root().mkpath(target_path);
	}

	return target_path;
}

// example
// i wanna import /home/user/dir
// my music library is in /home/user/Music
// i will type "chosen" into entry field
// i expect a directory /home/user/Music/chosen/dir in my music library

void ImportCopyThread::copy() {

	_copied_files = 0;
    _lst_copied_files.clear();
    _created_dirs.clear();
    _percent = 0;
	_v_md.clear();

	for(int i=0; i<_files.size(); i++){

		// insert into vector for db
		emit_percent(i, _files.size());

		if(_cancelled) return;

		// absolute path of an mp3 file
		QString filename = _files[i];


		// target path = /home/user/Music/chosen_dir/dir/subdir1/subdir2
		// or          = /home/user/Music/chosen_dir/, if only files
		// append chosendir to libdir and appends the "dir" string
		QString target_path = _create_target_path( _pd_map[filename], filename, _lib_dir, _chosen_dir );

		_created_dirs << target_path;

		// copy file
		QFile f(filename);
		QString filename_wo_folder = Helper::File::get_filename_of_path(filename);
		QString new_filename = target_path + QDir::separator() + filename_wo_folder;

		filename = Helper::File::get_absolute_filename(filename);
		new_filename = Helper::File::get_absolute_filename(new_filename);

		bool copied = true;

		if(filename != new_filename){
			copied = f.copy(new_filename);
		}

		else if(copied) {
			_lst_copied_files << new_filename;
		}

		else{
			continue;
		}

		if(!Helper::File::is_soundfile(filename)){
			continue;
		}

		_copied_files++;

		MetaData md = _md_map[filename];
		md.set_filepath( new_filename );
		_v_md << std::move(md);
    }
}

void ImportCopyThread::rollback() {

    int n_operations = _lst_copied_files.size() + _created_dirs.size();
    int n_ops_todo = n_operations;
    int percent;

	QDir dir(_lib_dir);

	for(const QString& f : _lst_copied_files) {
        QFile file(f);
        file.remove();
        percent = ((n_ops_todo--) * (_percent * 1000)) / (n_operations);

        emit sig_progress(percent/ 1000);
    }


	for(QString d : _created_dirs) {
        d.remove(_lib_dir);
		while(d.startsWith(QDir::separator())) {
			d.remove(0,1);
		}

        dir.rmpath(d);
    	percent = ((n_ops_todo--) * (_percent * 1000)) / (n_operations);

        emit sig_progress(percent/ 1000);
    }

    _percent = 0;
    _n_files = 0;
    _copied_files = 0;

    _lst_copied_files.clear();
	_created_dirs.clear();
	_v_md.clear();
}


void ImportCopyThread::run() {

    _cancelled = false;
	if(_mode == Mode::Copy){
		copy();
	}

	else if(_mode == Mode::Rollback){
		rollback();
	}
}


void ImportCopyThread::set_vars( const QString& target_dir,
								 const QStringList& files,
								 const QMap<QString, MetaData>& md_map,
								 const QMap<QString, QString>& pd_map )
{
	_lib_dir = _settings->get(Set::Lib_Path);

	_chosen_dir = target_dir;
	_files = files;

	_md_map = md_map;
	_pd_map = pd_map;

}

void ImportCopyThread::set_cancelled() {
    _cancelled = true;
}

bool ImportCopyThread::get_cancelled() const
{
    return _cancelled;
}

int ImportCopyThread::get_n_files() const
{
	return _v_md.size();
}

int ImportCopyThread::get_copied_files() const
{
    return _copied_files;
}

MetaDataList ImportCopyThread::get_metadata() const
{
	return _v_md;
}

void ImportCopyThread::set_mode(ImportCopyThread::Mode mode) {
    _mode = mode;
}

ImportCopyThread::Mode ImportCopyThread::get_mode() const
{
	return _mode;
}

