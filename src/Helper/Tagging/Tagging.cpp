/* id3.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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
#include "ID3v2/Popularimeter.h"
#include "ID3v2/Discnumber.h"
#include "ID3v2/Cover.h"
#include "ID3v2/AlbumArtist.h"
#include "Xiph/AlbumArtist.h"
#include "Xiph/PopularimeterFrame.h"
#include "Xiph/DiscnumberFrame.h"
#include "MP4/AlbumArtist.h"
#include "MP4/Cover.h"
#include "MP4/DiscnumberFrame.h"
#include "MP4/PopularimeterFrame.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/Genre.h"
#include "Helper/Logger/Logger.h"

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/oggfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/flacfile.h>
#include <taglib/tbytevector.h>
#include <taglib/tbytevectorstream.h>
#include <taglib/id3v1tag.h>
#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>

#include <QFile>
#include <QFileInfo>
#include <QImage>


static Tagging::TagType tag_type_from_fileref(const TagLib::FileRef& f);

bool Tagging::is_valid_file(const TagLib::FileRef& f)
{
	if( f.isNull() ||
		!f.tag() ||
		!f.file() ||
		!f.file()->isValid() )
	{
		return false;
	}

	return true;
}

bool Tagging::getMetaDataOfFile(MetaData& md, Tagging::Quality quality)
{
	bool success;

	QFileInfo fi(md.filepath());
	md.filesize = fi.size();

	TagLib::AudioProperties::ReadStyle read_style;
	bool read_audio_props=true;

	switch(quality)
	{
		case Tagging::Quality::Quality:
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

	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << md.filepath();
		return false;
	}

	TagLib::Tag* tag = f.tag();
	Tagging::TagType tag_type = tag_type_from_fileref(f);

	QString artist = QString::fromUtf8(tag->artist().toCString(true));
	QString album = QString::fromUtf8(tag->album().toCString(true));
	QString title = QString::fromUtf8(tag->title().toCString(true));
	QString genre = QString::fromUtf8(tag->genre().toCString(true));

	QString album_artist;
	Models::Discnumber discnumber;
	Models::Popularimeter popularimeter;
	if(tag_type == Tagging::TagType::ID3v2)
	{
		ID3v2::AlbumArtistFrame album_artist_frame(f);
		success = album_artist_frame.read(album_artist);
		if(success){
			md.set_album_artist(album_artist);
		}

		ID3v2::PopularimeterFrame popularimeter_frame(f);
		success = popularimeter_frame.read(popularimeter);
		if(success){
			md.rating = popularimeter.get_rating();
		}

		ID3v2::DiscnumberFrame discnumber_frame(f);
		success = discnumber_frame.read(discnumber);
		if(success){
			md.discnumber = discnumber.disc;
			md.n_discs = discnumber.n_discs;
		}
	}

	else if(tag_type == Tagging::TagType::Xiph)
	{
		Xiph::AlbumArtistFrame album_artist_frame(tag);
		success = album_artist_frame.read(album_artist);
		if(success){
			md.set_album_artist(album_artist);
		}

		Xiph::PopularimeterFrame popularimeter_frame(tag);
		success = popularimeter_frame.read(popularimeter);
		if(success){
			md.rating = popularimeter.get_rating();
		}

		Xiph::DiscnumberFrame discnumber_frame(tag);
		success = discnumber_frame.read(discnumber);
		if(success){
			md.discnumber = discnumber.disc;
			md.n_discs = discnumber.n_discs;
		}
	}

	else if(tag_type == Tagging::TagType::MP4)
	{
		MP4::AlbumArtistFrame album_artist_frame(tag);
		success = album_artist_frame.read(album_artist);
		if(success){
			md.set_album_artist(album_artist);
		}


		MP4::DiscnumberFrame discnumber_frame(tag);
		success = discnumber_frame.read(discnumber);
		if(success){
			md.discnumber = discnumber.disc;
			md.n_discs = discnumber.n_discs;
		}

		MP4::PopularimeterFrame popularimeter_frame(tag);

		success = popularimeter_frame.read(popularimeter);
		if(success){
			md.rating = popularimeter.get_rating();
		}

		//sp_log(Log::Debug) << "Read rating " << (int) md.rating << ": " << success;
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
	genres = genre_str.split(QRegExp(",|/|;"));
	for(int i=0; i<genres.size(); i++) {
		genres[i] = genres[i].trimmed();
	}

	genres.removeDuplicates();
	genres.removeAll("");

	md.album = album;
	md.artist = artist;
	md.title = title;
	md.length_ms = length;
	md.year = year;
	md.track_num = track;
	md.bitrate = bitrate;
	md.set_genres(genres);
	md.discnumber = discnumber.disc;
	md.n_discs = discnumber.n_discs;
	md.rating = popularimeter.get_rating();

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


bool Tagging::setMetaDataOfFile(const MetaData& md)
{
	QString filepath = md.filepath();
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));

	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << md.filepath();
		return false;
	}

	bool success;

	TagLib::String album(md.album.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String artist(md.artist.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String title(md.title.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String genre(md.genres_to_string().toUtf8().data(), TagLib::String::UTF8);
	TagLib::Tag* tag = f.tag();
	Tagging::TagType tag_type = tag_type_from_fileref(f);

	tag->setAlbum(album);
	tag->setArtist(artist);
	tag->setTitle(title);
	tag->setGenre(genre);
	tag->setYear(md.year);
	tag->setTrack(md.track_num);

	Models::Popularimeter popularimeter("sayonara player", 0, 0);
	popularimeter.set_rating(md.rating);
	Models::Discnumber discnumber(md.discnumber, md.n_discs);

	if(tag_type == Tagging::TagType::ID3v2)
	{
		ID3v2::PopularimeterFrame popularimeter_frame(f);
		popularimeter_frame.write(popularimeter);

		ID3v2::DiscnumberFrame discnumber_frame(f);
		discnumber_frame.write(discnumber);

		ID3v2::AlbumArtistFrame album_artist_frame(f);
		album_artist_frame.write(md.album_artist());
	}

	else if(tag_type == Tagging::TagType::Xiph)
	{
		Xiph::PopularimeterFrame popularimeter_frame(tag);
		popularimeter_frame.write(popularimeter);

		Xiph::DiscnumberFrame discnumber_frame(tag);
		discnumber_frame.write(discnumber);

		Xiph::AlbumArtistFrame album_artist_frame(tag);
		album_artist_frame.write(md.album_artist());
	}

	else if(tag_type == Tagging::TagType::MP4)
	{
		MP4::AlbumArtistFrame album_artist_frame(tag);
		album_artist_frame.write(md.album_artist());

		MP4::DiscnumberFrame discnumber_frame(tag);
		discnumber_frame.write(discnumber);

		MP4::PopularimeterFrame popularimeter_frame(tag);
		success = popularimeter_frame.write(popularimeter);
	}

	success = f.save();
	if(!success){
		sp_log(Log::Warning) << "Could not save " << md.filepath();
	}

	return true;
}


bool Tagging::write_cover(const MetaData& md, const QImage& cover)
{
	bool success;
	QString filepath = Helper::get_sayonara_path() + "tmp.png";

	success = cover.save(filepath);
	if(!success){
		sp_log(Log::Warning) << "Can not save temporary cover: " << filepath;
		sp_log(Log::Warning) << "Is image valid? " << !cover.isNull();
		return false;
	}

	success = write_cover(md, filepath);
	QFile::remove(filepath);

	return success;
}


bool Tagging::write_cover(const MetaData& md, const QString& cover_image_path)
{
	QString error_msg = "Cannot save cover. ";
	QString filepath = md.filepath();
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));
	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << md.filepath();
		return false;
	}

	QByteArray data;
	bool success = Helper::File::read_file_into_byte_arr(cover_image_path, data);
	if(data.isEmpty() || !success){
		sp_log(Log::Warning) << error_msg << "No image data available: " << cover_image_path;
		return false;
	}

	QString mime_type = "image/";
	QString ext = Helper::File::get_file_extension(cover_image_path);
	if(ext.compare("jpg", Qt::CaseInsensitive) == 0){
		mime_type += "jpeg";
	}

	else if(ext.compare("png", Qt::CaseInsensitive) == 0){
		mime_type += "png";
	}

	else{
		sp_log(Log::Warning) << error_msg << "Unknown mimetype: '" << ext << "'";
		return false;
	}

	Models::Cover cover(mime_type, data);
	Tagging::TagType tag_type = get_tag_type(md.filepath());
	if(tag_type == Tagging::TagType::ID3v2){
		ID3v2::CoverFrame cover_frame(f);
		cover_frame.write(cover);
	}

	else if(tag_type == Tagging::TagType::MP4){
		MP4::CoverFrame cover_frame(f.tag());
		if(!cover_frame.write(cover)){
			return false;
		}
	}

	return f.save();
}

bool Tagging::extract_cover(const MetaData &md, QByteArray& cover_data, QString& mime_type)
{
	QString filepath = md.filepath();
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));

	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << md.filepath();
		return false;
	}

	Models::Cover cover;
	Tagging::TagType tag_type = get_tag_type(md.filepath());
	switch(tag_type){

		case Tagging::TagType::ID3v2:
			{
				ID3v2::CoverFrame cover_frame(f);

				if(!cover_frame.is_frame_found()){
					return false;
				}

				cover_frame.read(cover);
			}

			break;

		case Tagging::TagType::MP4:
			{
				MP4::CoverFrame cover_frame(f.tag());
				if(!cover_frame.read(cover)){
					return false;
				}
			}

			break;

		default:
			return false;
	}

	cover_data = cover.image_data;
	mime_type = cover.mime_type;

	return !(cover_data.isEmpty());
}

static Tagging::TagType tag_type_from_fileref(const TagLib::FileRef& f)
{
	TagLib::MPEG::File* mpg = dynamic_cast<TagLib::MPEG::File*>(f.file());
	if(mpg)
	{
		if(mpg->hasID3v2Tag()){
			return Tagging::TagType::ID3v2;
		}

		if(mpg->hasID3v1Tag()){
			return Tagging::TagType::ID3v1;
		}
	}

	TagLib::FLAC::File* flac = dynamic_cast<TagLib::FLAC::File*>(f.file());
	if(flac)
	{
		if(flac->hasID3v2Tag()){
			return Tagging::TagType::ID3v2;
		}

		if(flac->hasID3v1Tag()){
			return Tagging::TagType::ID3v1;
		}
	}

	TagLib::Tag* tag = f.tag();
	if(dynamic_cast<TagLib::ID3v2::Tag*>(tag) != nullptr){
		return Tagging::TagType::ID3v2;
	}

	if(dynamic_cast<TagLib::ID3v1::Tag*>(tag) != nullptr){
		return Tagging::TagType::ID3v1;
	}

	if(dynamic_cast<TagLib::Ogg::XiphComment*>(tag) != nullptr){
		return Tagging::TagType::Xiph;
	}

	if(dynamic_cast<TagLib::MP4::Tag*>(tag) != nullptr){
		return Tagging::TagType::MP4;
	}

	return Tagging::TagType::Unsupported;
}


Tagging::TagType Tagging::get_tag_type(const QString &filepath)
{
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));
	if(!is_valid_file(f)){
		return Tagging::TagType::Unknown;
	}

	return tag_type_from_fileref(f);
}

QString Tagging::tag_type_to_string(Tagging::TagType type)
{
	switch(type){
		case Tagging::TagType::ID3v1:
			return "ID3v1";
		case Tagging::TagType::ID3v2:
			return "ID3v2";
		case Tagging::TagType::Xiph:
			return "Xiph";
		case Tagging::TagType::MP4:
			return "MP4";
		case Tagging::TagType::Unknown:
			return "Unknown";
		default:
			return "Partially unsupported";
	}
}

bool Tagging::is_cover_supported(const QString& filepath)
{
	Tagging::TagType type = get_tag_type(filepath);
	return (type == Tagging::TagType::ID3v2 || type == Tagging::TagType::MP4);
}
