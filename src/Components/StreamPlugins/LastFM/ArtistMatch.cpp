#include "ArtistMatch.h"
#include <QStringList>

#include <algorithm>
ArtistMatch::ArtistMatch()
{

}


ArtistMatch::ArtistMatch(const QString& artist_name)
{
	_artist = artist_name;
}


ArtistMatch::ArtistMatch(const ArtistMatch& other)
{
	_artist = other._artist;
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

void ArtistMatch::add(const ArtistDesc& artist, double match){
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

QMap<ArtistMatch::ArtistDesc, double> ArtistMatch::get(Quality q) const
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

QString ArtistMatch::to_string() const
{
	QStringList lst;

	for(const ArtistMatch::ArtistDesc& key : _very_good.keys()){
		lst << QString::number(_very_good[key]).left(5) + "\t" + key.to_string();
	}

	for(const ArtistMatch::ArtistDesc& key : _well.keys()){
		lst << QString::number(_well[key]).left(5) + "\t" + key.to_string();
	}

	for(const ArtistMatch::ArtistDesc& key : _poor.keys()){
		lst << QString::number(_poor[key]).left(5) + "\t" + key.to_string();
	}

	std::sort(lst.begin(), lst.end());
	return lst.join("\n");
}


ArtistMatch::ArtistDesc::ArtistDesc(const QString& artist_name, const QString& mbid)
{
	this->artist_name = artist_name;
	this->mbid = mbid;
}

bool ArtistMatch::ArtistDesc::operator ==(const ArtistMatch::ArtistDesc& other) const
{
	return (artist_name == other.artist_name);
}

bool ArtistMatch::ArtistDesc::operator <(const ArtistMatch::ArtistDesc& other) const
{
	return (artist_name < other.artist_name);
}

bool ArtistMatch::ArtistDesc::operator <=(const ArtistMatch::ArtistDesc& other) const
{
	return (artist_name <= other.artist_name);
}


QString ArtistMatch::ArtistDesc::to_string() const
{
	return mbid + "\t" + artist_name;
}
