/* FileUtils.cpp */

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

#include "Utils/FileUtils.h"
#include "Utils/Utils.h"
#include "Utils/Logger/Logger.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

#include <algorithm>

QString Util::File::clean_filename(const QString& path)
{
	QChar sep = QDir::separator();
	QString ret = path;
	while(ret.contains("/./") || ret.contains("\\.\\")){
		ret.replace("/./", sep);
		ret.replace("\\.\\", sep);
	}

	while(ret.contains("//") || ret.contains("\\\\")){
		ret.replace("//", sep);
		ret.replace("\\\\", sep);
	}

	ret.replace("\\", sep);

	if(ret.endsWith(sep))
	{
		ret.remove(ret.size() - 1, 1);
	}

	return ret;
}


QString Util::File::calc_file_extension(const QString& filename)
{
	return get_file_extension(filename);
}


void Util::File::remove_files_in_directory(const QString& dir_name)
{
	remove_files_in_directory(dir_name, QStringList());
}

void Util::File::remove_files_in_directory(const QString& dir_name, const QStringList& filters)
{
	bool success;
	QDir dir(dir_name);
	dir.setNameFilters(filters);

	QFileInfoList info_lst = dir.entryInfoList(
			(QDir::Filters)(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)
	);

	for(const QFileInfo& info : info_lst){
		QString path = info.absoluteFilePath();
		if(info.isDir())
		{
			remove_files_in_directory(path);
			QDir().rmdir(path);
		}

		else{
			QFile file(path);
			file.remove();
		}
	}

	QDir d = QDir::root();
	success = d.rmdir(dir_name);
	if(!success){
		sp_log(Log::Warning) << "Could not remove dir " << dir_name;
	}
}


void Util::File::delete_files(const QStringList& paths)
{
	QStringList sorted_paths = paths;
	std::sort(sorted_paths.begin(), sorted_paths.end(), [](const QString& str1, const QString& str2){
			return (str1.size() > str2.size());
	});

	for(const QString& path : sorted_paths)
	{
		QFileInfo info(path);
		if(!info.exists()){
			continue;
		}

		if(info.isDir()){
			remove_files_in_directory(path);
			QDir().rmdir(path);
		}

		else {
			QFile::remove(path);
		}
	}
}


QString Util::File::get_parent_directory(const QString& filename)
{
	QString ret = clean_filename(filename);
	int last_idx = ret.lastIndexOf(QDir::separator());

	if(last_idx >= 0){
		return ret.left(last_idx);
	}

	return ret;
}


QString Util::File::get_filename_of_path(const QString& path)
{
	QString ret = clean_filename(path);
	int last_idx = ret.lastIndexOf(QDir::separator());

	if(last_idx >= 0){
		return ret.mid(last_idx + 1);
	}

	return "";
}


void Util::File::split_filename(const QString& src, QString& path, QString& filename)
{
	path = Util::File::get_parent_directory(src);
	filename = Util::File::get_filename_of_path(src);
}


QStringList Util::File::get_parent_directories(const QStringList& files)
{
	QStringList folders;
	for(const QString& file : files)
	{
		QString folder = get_parent_directory(file);
		if(!folders.contains(folder)){
			folders << folder;
		}
	}

	return folders;
}


QString Util::File::get_absolute_filename(const QString& filename)
{
	QString f, d;
	QString re_str = QString("(.*)") + QDir::separator() + "(.+)";
	QRegExp re(re_str);
	if(re.indexIn(filename) >= 0){
		d = re.cap(1);
		f = re.cap(2);
		QDir dir(d);
		return dir.absoluteFilePath(f);
	}

	return clean_filename(filename);
}


QString Util::File::calc_filesize_str(uint64_t filesize)
{
	uint64_t kb = 1 << 10;  // 1024
	uint64_t mb = kb << 10;
	uint64_t gb = mb << 10;

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


bool Util::File::is_url(const QString& str)
{
	if(is_www(str)) return true;
	if(str.startsWith("file"), Qt::CaseInsensitive) return true;
	return false;
}


bool Util::File::is_www(const QString& str)
{
	if(str.startsWith("http://")) return true;
	else if(str.startsWith("https://")) return true;
	else if(str.startsWith("ftp://")) return true;
	else if(str.startsWith("itpc://")) return true;
	else if(str.startsWith("feed://")) return true;
	return false;
}

bool Util::File::is_dir(const QString& filename)
{
	if(!QFile::exists(filename)) return false;
	QFileInfo fileinfo(filename);
	return fileinfo.isDir();
}

bool Util::File::is_file(const QString& filename)
{
	if(!QFile::exists(filename)) return false;
	QFileInfo fileinfo(filename);
	return fileinfo.isFile();
}


bool Util::File::is_soundfile(const QString& filename)
{
	QStringList exts = Util::soundfile_extensions(true);

	return std::any_of(exts.begin(), exts.end(), [&filename](const QString& ext)
	{
		return (filename.toLower().endsWith(ext.right(4)));
	});
}


bool Util::File::is_playlistfile(const QString& filename)
{
	QStringList exts = Util::playlist_extensions(true);

	return std::any_of(exts.begin(), exts.end(), [&filename](const QString& ext)
	{
		return (filename.toLower().endsWith(ext.right(4)));
	});
}


bool Util::File::create_directories(const QString& path)
{
	if(QFile::exists(path)){
		return true;
	}

	QString cleaned_path = clean_filename(path);
	QStringList paths = cleaned_path.split(QDir::separator());
	QDir dir;
	if( is_absolute(cleaned_path) ){
		dir = QDir::root();
	}

	else{
		dir = QDir(".");
	}

	for(QString p : paths)
	{
		QString abs_path = dir.absoluteFilePath(p);

		if(QFile::exists(abs_path)){
			dir.cd(p);
			continue;
		}

		bool success = dir.mkdir(p);
		if(!success){
			return false;
		}

		dir.cd(p);
	}

	return true;
}


bool Util::File::is_absolute(const QString& filename)
{
	QDir dir(filename);
	return dir.isAbsolute();
}


bool Util::File::write_file(const QByteArray& arr, const QString& filename)
{
	QFile f(filename);
	if(!f.open(QFile::WriteOnly)){
		return false;
	}

	int64_t bytes_written = f.write(arr);

	f.close();

	return(bytes_written >= arr.size());
}


bool Util::File::read_file_into_byte_arr(const QString& filename, QByteArray& content)
{
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


bool Util::File::read_file_into_str(const QString& filename, QString& content)
{
	QFile file(filename);
	content.clear();
	if(!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	while (!file.atEnd()) {
		content.append(QString::fromUtf8(file.readLine()));
	}

	file.close();

	if(content.size() > 0 ) {
		return true;
	}

	return false;
}


QString Util::File::get_file_extension(const QString& filename)
{
	int last_dot = filename.lastIndexOf(".");
	if(last_dot < 0){
		return "";
	}

	return filename.mid(last_dot + 1);
}


bool Util::File::check_file(const QString& filepath)
{
	if(is_www(filepath)){
		return true;
	}

	return QFile::exists(filepath);
}

bool Util::File::create_symlink(const QString& source, const QString& target)
{
	QFile f(source);
	return f.link(target);
}

QString Util::File::get_common_directory(QString dir1, QString dir2)
{
	while(dir1.compare(dir2) != 0)
	{
		while(dir1.size() > dir2.size())
		{
			QDir d1(dir1);
			bool up = d1.cdUp();
			if(!up){
				return "";
			}
			dir1 = d1.absolutePath();
		}

		while(dir2.size() > dir1.size())
		{
			QDir d2(dir2);
			bool up = d2.cdUp();
			if(!up){
				return "";
			}

			dir2 = d2.absolutePath();
		}
	}

	return dir1;
}

QString Util::File::get_common_directory(const QStringList& paths)
{
	if(paths.isEmpty()){
		return QDir::rootPath();
	}

	if(paths.size() == 1){
		return QDir(paths[0]).absolutePath();
	}

	QString ret;
	QStringList absolute_paths;
	for(const QString& path : paths)
	{
		QString filename = get_absolute_filename(path);
		QFileInfo info(filename);
		if(info.isFile()){
			QDir d(filename);
			absolute_paths << d.absolutePath();
		}

		else if(info.isDir()){
			absolute_paths << get_absolute_filename(filename);
		}

		else if(info.isRoot()){
			return QDir::rootPath();
		}
	}

	if(absolute_paths.isEmpty()){
		return QDir::rootPath();
	}

	if(absolute_paths.size() == 1){
		return absolute_paths[0];
	}

	ret = absolute_paths[0];

	for(const QString& absolute_path : absolute_paths)
	{
		ret = get_common_directory(ret, absolute_path);
	}

	return ret;
}

QStringList Util::File::split_directories(const QString& path)
{
	QStringList ret;

	QString current_dir;
	QFileInfo fi(path);

	if(fi.isDir()) {
		current_dir = path;
	}

	else if(fi.isFile()){
		QString filename;
		split_filename(path, current_dir, filename);
	}

	while(!QDir(current_dir).isRoot())
	{
		QString last_dir;
		QString parent_dir;
		split_filename(current_dir, parent_dir, last_dir);

		ret << last_dir;
		current_dir = parent_dir;
	}

	return ret;
}
