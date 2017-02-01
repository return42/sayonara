#include "AlbumCoverModel.h"
#include <QUrl>


AlbumCoverModel::AlbumCoverModel(const QString& album_name, const QString& cover_path) :
	QObject()
{
	_album_name = album_name;
	_cover_path = cover_path;
}

AlbumCoverModel::~AlbumCoverModel() {}

QString AlbumCoverModel::coverPath() const
{
	return QUrl("file://" + _cover_path).toString();
}

void AlbumCoverModel::setCoverPath(const QString& coverPath)
{
	if (_cover_path == coverPath){
		return;
	}

	_cover_path = coverPath;

	emit coverPathChanged(_cover_path);
}

QString AlbumCoverModel::albumName() const
{
	return _album_name;
}

void AlbumCoverModel::setAlbumName(const QString &albumName)
{
	if(albumName == _album_name){
		return;
	}

	_album_name = albumName;
	emit albumNameChanged(_album_name);
}
