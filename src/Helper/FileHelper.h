/* FileHelper.h */

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



#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QString>
#include <QStringList>

namespace FileHelper
{
	/**
	 * @brief calc_file_extension
	 * @param filename
	 * @return
	 */
	QString calc_file_extension(const QString& filename);
	void remove_files_in_directory(const QString& dir_name, const QStringList& filters);
	QString get_parent_folder(const QString& path);
	QString get_filename_of_path(const QString& path);
	void split_filename(const QString& src, QString& path, QString& filename);
	QStringList extract_folders_of_files(const QStringList& list);
	QString get_absolute_filename(const QString& filename);


	/**
	 * @brief convert filesize to string
	 * @param filesize in bytes
	 * @return converted string
	 */
	QString calc_filesize_str(quint64 filesize);



	// Everything clear
	bool is_url(const QString& str);
	bool is_www(const QString& str);
	bool is_file(const QString& filename);
	bool is_dir(const QString& filename);
	bool is_soundfile(const QString& filename);
	bool is_playlistfile(const QString& filename);
	bool is_podcastfile(const QString& filename, const QByteArray& content);
}

#endif // FILEHELPER_H
