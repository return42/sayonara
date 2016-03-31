/* FileHelper.cpp */

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



#include "FileHelper.h"
#include "Helper.h"

#include <QDir>

QString Helper::File::calc_file_extension(const QString& filename) {

	return get_file_extension(filename);
}



void Helper::File::remove_files_in_directory(const QString& dir_name, const QStringList& filters) {

	QStringList file_list;
	QDir dir(dir_name);
	dir.setFilter(QDir::Files);

	if(filters.size() == 0) {
		QStringList tmp_list;
		tmp_list << "*";
		dir.setNameFilters(tmp_list);
	}

	else {
		dir.setNameFilters(filters);
	}

	file_list = dir.entryList();

	for(const QString& filename : file_list) {

		QFile file(dir.absoluteFilePath(filename));
		file.remove();
	}
}

QString Helper::File::get_parent_directory(const QString& filename) {

	QString ret;
	QString re_str = QString("(.*)") + QDir::separator() + ".+";
	QRegExp re(re_str);

	if(re.indexIn(filename) >= 0){
		ret = get_absolute_filename(re.cap(1));
	}

	return ret;
}

QString Helper::File::get_filename_of_path(const QString& path) {

	QString ret = path;

	while(ret.endsWith(QDir::separator())){
		ret.remove(path.size() - 1, 1);
	}

	ret.remove(Helper::File::get_parent_directory(path));
	ret.remove(QDir::separator());

	return get_absolute_filename(ret);
}

void Helper::File::split_filename(const QString& src, QString& path, QString& filename) {

	path = Helper::File::get_parent_directory(src);
	filename = Helper::File::get_filename_of_path(src);
}



QStringList Helper::File::get_parent_directories(const QStringList& files) {

	QStringList folders;
	for(const QString& file : files) {
		QString folder = get_parent_directory(file);
		if(!folders.contains(folder)){
			folders << folder;
		}
	}

	return folders;
}


QString Helper::File::get_absolute_filename(const QString& filename){

	QString f, d;
	QString re_str = QString("(.*)") + QDir::separator() + "(.+)";
	QRegExp re(re_str);
	if(re.indexIn(filename) >= 0){
		d = re.cap(1);
		f = re.cap(2);
		QDir dir(d);
		return dir.absoluteFilePath(f);
	}

	return filename;
}




QString Helper::File::calc_filesize_str(quint64 filesize) {
	quint64 kb = 1024;
	quint64 mb = kb * 1024;
	quint64 gb = mb * 1024;

	QString size;
	if(filesize > gb) {
		size = QString::number(filesize / gb) + "." + QString::number((filesize / mb) % gb).left(2)  + " GB";
	}

	else if (filesize > mb) {
		size = QString::number(filesize / mb) + "." + QString::number((filesize / kb) % mb).left(2)  + " MB";
	}

	else {
		size = QString::number( filesize / kb) + " KB";
	}

	return size;
}



bool Helper::File::is_url(const QString& str) {
	if(is_www(str)) return true;
	if(str.startsWith("file"), Qt::CaseInsensitive) return true;
	return false;
}


bool Helper::File::is_www(const QString& str) {

	if(str.startsWith("http://")) return true;
	else if(str.startsWith("https://")) return true;
	else if(str.startsWith("ftp://")) return true;
	else if(str.startsWith("itpc://")) return true;
	else if(str.startsWith("feed://")) return true;
	return false;
}

bool Helper::File::is_dir(const QString& filename) {
	if(!QFile::exists(filename)) return false;
	QFileInfo fileinfo(filename);
	return fileinfo.isDir();
}

bool Helper::File::is_file(const QString& filename) {
	if(!QFile::exists(filename)) return false;
	QFileInfo fileinfo(filename);
	return fileinfo.isFile();
}


bool Helper::File::is_soundfile(const QString& filename) {

	QStringList extensions = Helper::get_soundfile_extensions();
	for(const QString& extension : extensions) {
		if(filename.toLower().endsWith(extension.right(4))) {
			return true;
		}
	}

	return false;
}


bool Helper::File::is_podcastfile(const QString& filename, const QByteArray& content) {

	bool extension_correct = false;
	QStringList extensions = Helper::get_podcast_extensions();
	for(const QString& extension : extensions) {
		if(filename.toLower().endsWith(extension.right(4).toLower())) {
			extension_correct = true;
			break;
		}
	}

	if(!extension_correct){
		return false;
	}

	QString header = content.left(content.size());
	if(content.size() > 1024) header = content.left(1024);


	if(header.contains("<rss")) {
		return true;
	}

	return false;
}



bool Helper::File::is_playlistfile(const QString& filename) {
	QStringList extensions = Helper::get_playlistfile_extensions();
	for(const QString& extension : extensions) {
		if(filename.toLower().endsWith(extension.right(4).toLower())) {
			return true;
		}
	}

	return false;
}






void Helper::File::create_directories(const QString& path)
{

	if(QFile::exists(path)){
		return;
	}

	QStringList paths = path.split(QDir::separator());
	QDir dir = QDir::root();

	for(QString p : paths){

		QString abs_path = dir.absoluteFilePath(p);

		if(QFile::exists(abs_path)){
			dir.cd(p);
			continue;
		}

		bool success = dir.mkdir(p);
		if(!success){
			break;
		}

		dir.cd(p);
	}
}


bool Helper::File::is_absolute(const QString& filename)
{
	QDir dir(filename);
	return dir.isAbsolute();
}


bool Helper::File::write_file(const QByteArray& arr, const QString& filename)
{
	QFile f(filename);
	if(!f.open(QFile::WriteOnly)){
		return false;
	}

	qint64 bytes_written = f.write(arr);

	f.close();

	return(bytes_written >= arr.size());
}


bool Helper::File::read_file_into_byte_arr(const QString& filename, QByteArray& content){
	QFile file(filename);
	content.clear();


	if(!file.open(QIODevice::ReadOnly)){
		return false;
	}

	while(!file.atEnd()){
		QByteArray arr = file.read(4096);
		content.append(arr);
	}

	file.close();

	return (content.size() > 0);
}


bool Helper::File::read_file_into_str(const QString& filename, QString& content) {

	QFile file(filename);
	content.clear();
	if(!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	while (!file.atEnd()) {
		content.append(file.readLine());
	}

	file.close();

	if(content.size() > 0 ) {
		return true;
	}

	return false;

}



QString Helper::File::get_file_extension(const QString& filename)
{
	QString ret;
	QRegExp re(".*\\.(.+)");
	if(re.indexIn(filename) >= 0){
		ret = re.cap(0);
	}

	return ret;
}
