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
#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"

#include <QMap>
#include <QVariant>
#include <QModelIndex>
#include <QMimeData>
#include <QUrl>

FileListModel::FileListModel(QObject* parent) :
	AbstractSearchListModel(parent)
{

}

FileListModel::~FileListModel() {}

void FileListModel::set_parent_directory(const QString& dir)
{
	_files.clear();

	QDir base_dir(dir);
	QStringList extensions;
	extensions = Helper::get_soundfile_extensions();
	extensions << Helper::get_playlistfile_extensions();
	extensions << "*";

	DirectoryReader reader;
	reader.set_filter(extensions);
	reader.get_files_in_dir(base_dir, _files);

	beginInsertRows(QModelIndex(), 0, rowCount() - 1);
	endInsertRows();

	emit dataChanged( 
			index(0,0), 
			index(rowCount() - 1, 0) 
	);
}

QStringList FileListModel::get_files() const
{
	return _files;
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return _files.size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole)
	{
		int row = index.row();
		if(row >= 0 && row < _files.size()){
			return Helper::File::get_filename_of_path(_files[row]);
		}
	}

	return QVariant();
}


QModelIndex FileListModel::getFirstRowIndexOf(const QString& substr)
{
	return getNextRowIndexOf(substr, 0);
}

QModelIndex FileListModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	QString converted_string = Library::convert_search_string(substr, search_mode());

	for(int i=0; i<_files.size(); i++)
	{
		cur_row = (cur_row + 1) % rowCount();

		QString converted_filepath = Library::convert_search_string(_files[cur_row], search_mode());
		if( converted_filepath.contains(converted_string) ){
			return index(cur_row, 0);
		}
	}

	return QModelIndex();
}

QModelIndex FileListModel::getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)
	QString converted_string = Library::convert_search_string(substr, search_mode());

	for(int i=0; i<_files.size(); i++)
	{
		cur_row = (cur_row - 1);
		if(cur_row == -1){
			cur_row = rowCount() - 1;
		}

		QString converted_filepath = Library::convert_search_string(_files[cur_row], search_mode());
		if( converted_filepath.contains(converted_string) ){
			return index(cur_row, 0);
		}
	}

	return QModelIndex();
}

QMap<QChar, QString> FileListModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}


QMimeData* FileListModel::mimeData(const QModelIndexList &indexes) const
{
	Q_UNUSED(indexes)

	QList<QUrl> urls;

	for(const QModelIndex& idx : indexes)
	{
		QString filename = _files[idx.row()];
		if(Helper::File::is_soundfile(filename) || Helper::File::is_playlistfile(filename)){
			urls << QUrl::fromLocalFile(filename);
		}
	}

	if(urls.isEmpty()){
		return nullptr;
	}

	QMimeData* data = new QMimeData();
	data->setUrls(urls);

	return data;
}


Qt::ItemFlags FileListModel::flags(const QModelIndex& index) const
{
	QString file = _files[index.row()];
	if(Helper::File::is_soundfile(file) || Helper::File::is_playlistfile(file))
	{
		return (Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
	}

	return Qt::NoItemFlags;
}
