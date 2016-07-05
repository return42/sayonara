#ifndef WWWPLAYLISTPARSER_H
#define WWWPLAYLISTPARSER_H

#include <QObject>
#include "Helper/MetaData/MetaDataList.h"

class WWWPlaylistParser : public QObject
{
	Q_OBJECT

signals:
	void sig_playlist_parsed(bool success);

public:
	explicit WWWPlaylistParser(QObject *parent = nullptr);

	void start(const QString& playlist_url);
	void start(const QStringList& playlist_urls);
	MetaDataList get_metadata() const;


private slots:
	void content_fetched(bool success);

private:
	MetaDataList	_v_md;
	QStringList		_urls;

private:
	void fetch_next_url();

};




#endif // WWWPLAYLISTPARSER_H
