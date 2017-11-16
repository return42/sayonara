/* FileListModel.cpp */

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

#include "FileListModel.h"
#include "Components/DirectoryReader/DirectoryReader.h"
#include "Components/Library/LibraryManager.h"

#include "Utils/Utils.h"
#include "Utils/FileUtils.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Library/LibraryInfo.h"
#include "GUI/Utils/CustomMimeData.h"
#include "GUI/Utils/MimeDataUtils.h"

#include <QMap>
#include <QVariant>
#include <QModelIndex>
#include <QMimeData>
#include <QUrl>

struct FileListModel::Private
{
	QString		parent_directory;
	QString		parent_directory_origin;

	LibraryId	library_id;
	QStringList files;

	Private() :
		library_id(-1)
	{}
};

FileListModel::FileListModel(QObject* parent) :
	SearchableListModel(parent)
{
	m = Pimpl::make<Private>();
}

FileListModel::~FileListModel() {}


void FileListModel::set_parent_directory(LibraryId id, const QString& dir)
{
	Library::Info info = Library::Manager::instance()->library_info(id);

	m->files.clear();
	m->library_id = id;

	m->parent_directory = dir;
	m->parent_directory_origin = dir;
	m->parent_directory_origin.remove(info.symlink_path());
	if(m->parent_directory.startsWith("/")){
		m->parent_directory.remove(0, 1);
	}

	int old_rowcount = rowCount();

	QDir base_dir(dir);
	QStringList extensions;
	extensions = Util::soundfile_extensions();
	extensions << Util::playlist_extensions();
	extensions << "*";

	DirectoryReader reader;
	reader.set_filter(extensions);
	reader.get_files_in_dir(base_dir, m->files);

	if(m->files.size() > old_rowcount){
		beginInsertRows(QModelIndex(), old_rowcount, m->files.size());
		endInsertRows();
	}

	else if(m->files.size() < old_rowcount)
	{
		beginRemoveRows(QModelIndex(), m->files.size(), old_rowcount);
		endRemoveRows();
	}

	emit dataChanged(
			index(0,0),
			index(m->files.size() - 1, 0)
	);
}

QString FileListModel::parent_directory() const
{
	return m->parent_directory;
}

QString FileListModel::parent_directory_origin() const
{
	return m->parent_directory_origin;
}

QString FileListModel::filepath_origin(const QModelIndex& index) const
{
	QString filepath = m->files[index.row()];
	filepath.replace(m->parent_directory_origin, m->parent_directory);

	return filepath;
}

QStringList FileListModel::files() const
{
	return m->files;
}

LibraryId FileListModel::library_id() const
{
	return m->library_id;
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return m->files.size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole)
	{
		int row = index.row();
		if(row >= 0 && row < m->files.size()){
			return Util::File::get_filename_of_path(m->files[row]);
		}
	}

	return QVariant();
}


QModelIndex FileListModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	QString converted_string = Library::Util::convert_search_string(substr, search_mode());

	for(int i=0; i<m->files.size(); i++)
	{
		int row = (cur_row + i) % m->files.size();

		QString converted_filepath = Library::Util::convert_search_string(m->files[row], search_mode());
		if( converted_filepath.contains(converted_string) ){
			return index(row, 0);
		}
	}

	return QModelIndex();
}

QModelIndex FileListModel::getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)
	QString converted_string = Library::Util::convert_search_string(substr, search_mode());

	for(int i=0; i<m->files.size(); i++)
	{
		int row = (cur_row - i);
		if(row == -1){
			row = m->files.size()	- 1;
		}

		QString converted_filepath = Library::Util::convert_search_string(m->files[row], search_mode());
		if( converted_filepath.contains(converted_string) ){
			return index(row, 0);
		}
	}

	return QModelIndex();
}

QMimeData* FileListModel::mimeData(const QModelIndexList& indexes) const
{
	Q_UNUSED(indexes)

	QList<QUrl> urls;

	for(const QModelIndex& idx : indexes)
	{
		QString filename = filepath_origin(idx);
		if(Util::File::is_soundfile(filename) || Util::File::is_playlistfile(filename)){
			urls << QUrl::fromLocalFile(filename);
		}
	}

	if(urls.isEmpty()){
		return nullptr;
	}

	CustomMimeData* data = new CustomMimeData(this);
	data->setUrls(urls);

	return data;
}


Qt::ItemFlags FileListModel::flags(const QModelIndex& index) const
{
	QString file = m->files[index.row()];
	if(Util::File::is_soundfile(file) || Util::File::is_playlistfile(file))
	{
		return (Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
	}

	return Qt::NoItemFlags;
}
