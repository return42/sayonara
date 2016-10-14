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

QVariant FileListModel::data(const QModelIndex& idx, int role) const
{
	if(role != Qt::DisplayRole){
		return QVariant();
	}

	int row = idx.row();
	return Helper::File::get_filename_of_path(_files[row]);
}

QModelIndex FileListModel::getFirstRowIndexOf(QString substr)
{
	return getNextRowIndexOf(substr, 0);
}

QModelIndex FileListModel::getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	for(int i=0; i<_files.size(); i++)
	{
		cur_row = (cur_row + 1) % rowCount();
		if( _files[cur_row].contains(substr, Qt::CaseInsensitive) ){
			return index(cur_row, 0);
		}
	}

	return QModelIndex();
}

QModelIndex FileListModel::getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	for(int i=0; i<_files.size(); i++)
	{
		cur_row = (cur_row - 1);
		if(cur_row == -1){
			cur_row = rowCount() - 1;
		}

		if( _files[cur_row].contains(substr, Qt::CaseInsensitive) ){
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
		urls << QUrl::fromLocalFile(filename);
	}

	QMimeData* data = new QMimeData();
	data->setUrls(urls);

	return data;
}


Qt::ItemFlags FileListModel::flags(const QModelIndex& index) const
{
	Q_UNUSED(index)

	return (Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
}

