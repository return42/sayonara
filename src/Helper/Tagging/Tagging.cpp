/* id3.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


#include "Tagging.h"
#include "Frames/Popularimeter.h"
#include "Frames/Discnumber.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Database/DatabaseConnector.h"

#include <QDir>
#include <QFile>

#include <taglib/tbytevector.h>
#include <taglib/tbytevectorstream.h>

bool Tagging::getMetaDataOfFile(MetaData& md, Tagging::Quality quality) {

	bool success;

	QFileInfo fi(md.filepath());
	md.filesize = fi.size();

	TagLib::AudioProperties::ReadStyle read_style;
	bool read_audio_props=true;

	switch(quality){
		case Taggint::Quality::Quality:
			read_style = TagLib::AudioProperties::Accurate;
			break;
		case Tagging::Quality::Standard:
			read_style = TagLib::AudioProperties::Average;
			break;
		case Tagging::Quality::Fast:
			read_style = TagLib::AudioProperties::Fast;
			break;
		case Tagging::Quality::Dirty:
			read_style = TagLib::AudioProperties::Fast;
			read_audio_props = false;
			break;
		default:
			read_style = TagLib::AudioProperties::Average;
	};

	TagLib::FileRef f(
				TagLib::FileName(md.filepath().toUtf8()),
				read_audio_props,
				read_style
	);

	if(f.isNull() || !f.tag() || !f.file()->isValid() || !f.file()->isReadable(md.filepath().toUtf8()) ) {
		sp_log(Log::Info) << md.filepath() << ": Something's wrong with this file";
		return false;
	}

	TagLib::Tag* tag = f.tag();

	QString artist = QString::fromUtf8(tag->artist().toCString(true));
	QString album = QString::fromUtf8(tag->album().toCString(true));
	QString title = QString::fromUtf8(tag->title().toCString(true));
	QString genre = QString::fromUtf8(tag->genre().toCString(true));
	QString comment = QString::fromUtf8(tag->comment().toCString(true));

	ID3v2Frame::PopularimeterFrame popularimeter_frame(&f);
	ID3v2Frame::Popularimeter popularimeter;
	success = popularimeter_frame.read(popularimeter);
	if(success){
		md.rating = popularimeter.get_sayonara_rating();
	}

	ID3v2Frame::DiscnumberFrame discnumber_frame(&f);
	ID3v2Frame::Discnumber discnumber;
	success = discnumber_frame.read(discnumber);
	if(success){
		md.discnumber = discnumber.disc;
		md.n_discs = discnumber.n_discs;
	}

	uint year = tag->year();
	uint track = tag->track();

	int bitrate=0;
	int length=0;

	if( quality != Tagging::Quality::Dirty ){
		bitrate = f.audioProperties()->bitrate() * 1000;
		length = f.audioProperties()->length() * 1000;
	}


    QStringList genres;
	QString genre_str = Helper::cvt_str_to_first_upper(genre);
    genres = genre_str.split(QRegExp(",|/|;|\\."));
    for(int i=0; i<genres.size(); i++) {
        genres[i] = genres[i].trimmed();
    }

	md.album = album;
	md.artist = artist;
	md.title = title;
	md.length_ms = length;
	md.year = year;
	md.track_num = track;
	md.bitrate = bitrate;
    md.genres = genres;
	md.discnumber = discnumber.disc;
	md.n_discs = discnumber.n_discs;
	md.comment = comment;
	md.rating = popularimeter.get_sayonara_rating();

	if(md.title.length() == 0) {
		QString dir, filename;
		Helper::File::split_filename(md.filepath(), dir, filename);

		if(filename.size() > 4){
			filename = filename.left(filename.length() - 4);
		}

		md.title = filename;
	}

	return true;
}


bool Tagging::setMetaDataOfFile(const MetaData& md) {

	QString filepath = md.filepath();
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));
	if(f.isNull() || !f.tag() || !f.file()->isValid() || !f.file()->isWritable(filepath.toUtf8()) ) {
        sp_log(Log::Info) << "ID3 cannot save";
		return false;
	}

	TagLib::String album(md.album.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String artist(md.artist.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String title(md.title.toUtf8().data(), TagLib::String::UTF8);
    TagLib::String genre(md.genres.join(",").toUtf8().data(), TagLib::String::UTF8);

	f.tag()->setAlbum(album);
	f.tag()->setArtist(artist);
	f.tag()->setTitle(title);
	f.tag()->setGenre(genre);
	f.tag()->setYear(md.year);
	f.tag()->setTrack(md.track_num);

	ID3v2Frame::Popularimeter popularimeter("sayonara player", 0, 0);
	ID3v2Frame::PopularimeterFrame popularimeter_frame(&f);
	popularimeter.set_sayonara_rating(md.rating);
	popularimeter_frame.write(popularimeter);

	ID3v2Frame::Discnumber discnumber(md.discnumber, md.n_discs);
	ID3v2Frame::DiscnumberFrame discnumber_frame(&f);
	discnumber_frame.write(discnumber);

	f.save();

	return true;
}
