#ifndef LFMSIMARTISTSPARSER_H
#define LFMSIMARTISTSPARSER_H

#include "ArtistMatch.h"

#include <QFile>
#include <QMap>

class LFMSimArtistsParser
{

private:
	ArtistMatch				_artist_match;
	QString					_artist_name;

	QByteArray              _data;


private:
	void parse_document();

public:

	LFMSimArtistsParser(const QString& artist_name, const QByteArray& arr);
	LFMSimArtistsParser(const QString& artist_name, const QString& filename);

	ArtistMatch get_artist_match() const;


};

#endif // LFMSIMARTISTSPARSER_H
