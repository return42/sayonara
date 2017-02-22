#ifndef ABSTRACTCOVERFETCHER_H
#define ABSTRACTCOVERFETCHER_H

class QString;
class QByteArray;
class QStringList;

class CoverFetcherInterface
{

public:
	virtual bool can_fetch_cover_directly() const=0;
	virtual QStringList calc_addresses_from_website(const QByteArray& website) const=0;
	virtual QString get_keyword() const=0;

	virtual QString get_artist_address(const QString& artist) const=0;
	virtual QString get_album_address(const QString& artist, const QString& album) const=0;
	virtual QString get_search_address(const QString& str) const=0;

	virtual bool is_search_supported() const=0;
	virtual bool is_album_supported() const=0;
	virtual bool is_artist_supported() const=0;

	virtual int get_estimated_size() const=0;
};

#endif // ABSTRACTCOVERFETCHER_H
