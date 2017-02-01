#ifndef ALBUMCOVERMODEL_H
#define ALBUMCOVERMODEL_H

#include <QObject>

class AlbumCoverModel :
		public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString coverPath READ coverPath WRITE setCoverPath NOTIFY coverPathChanged)
	Q_PROPERTY(QString albumName READ albumName WRITE setAlbumName NOTIFY albumNameChanged)

public:
	AlbumCoverModel(const QString& album_name, const QString& cover_path);
	~AlbumCoverModel();

private:
	QString _cover_path;
	QString _album_name;

signals:
	void coverPathChanged(const QString& coverPath);
	void albumNameChanged(const QString& albumName);

public:
	QString coverPath() const;
	QString albumName() const;

public slots:
	void setCoverPath(const QString& coverPath);
	void setAlbumName(const QString& albumName);
};

#endif // ALBUMCOVERMODEL_H
