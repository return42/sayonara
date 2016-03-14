#include "ArtistMatch.h"

ArtistMatch::ArtistMatch()
{

}


ArtistMatch::ArtistMatch(const QString& artist_name)
{
	_artist = artist_name;
}


ArtistMatch::ArtistMatch(const ArtistMatch& other)
{
	_very_good = other._very_good;
	_well = other._well;
	_poor = other._poor;
}



bool ArtistMatch::is_valid() const {
	return  (
				_very_good.size() > 0 ||
				_well.size() > 0  ||
				_poor.size() > 0
				);
}

bool ArtistMatch::operator ==(const ArtistMatch& am) const {
	return (_artist == am._artist);
}

void ArtistMatch::add(const QString& artist, double match){
	if(match > 0.15) {
		_very_good[artist] = match;
	}

	else if(match > 0.05) {
		_well[artist] = match;
	}

	else {
		_poor[artist] = match;
	}
}

QMap<QString, double> ArtistMatch::get(Quality q) const
{
	switch(q) {
		case Quality::Poor:
			return _poor;
		case Quality::Well:
			return _well;
		case Quality::Very_Good:
			return _very_good;
	}

	return _very_good;
}

QString ArtistMatch::get_artist_name() const
{
	return _artist;
}