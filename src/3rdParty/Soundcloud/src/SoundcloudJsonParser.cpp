/* SoundcloudJsonParser.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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



#include "SoundcloudJsonParser.h"


void remove_first_and_last(QByteArray& data, char first, char last){


	if(data.startsWith(first)){
		data.remove(0, 1);
	}

	if(data.endsWith(last)){
		data.remove(data.size() - 1, 1);
	}
}


SoundcloudJsonParser::SoundcloudJsonParser(const QByteArray& content) : QObject()
{
	_content = content;
}

SoundcloudJsonParser::~SoundcloudJsonParser()
{

}


int	SoundcloudJsonParser::find_value_end(const QByteArray& content, int start_at){

	int quote_counter = 0;

	for(int i=start_at; i<content.size(); i++){
		QChar c = content[i];

		if(c == '\"'){
			if(i > 0 && content[i - 1] == '\\'){
				;
			}
			else {
				quote_counter ++;
			}
		}

		if(quote_counter % 2 == 0 && c == ','){
			return i;
		}

		if(c == '}'){
			return i;
		}
	}

	return content.size() - 1;
}


int	SoundcloudJsonParser::find_block_end(const QByteArray& content, int start_at){

	int quote_counter = 0;

	for(int i=start_at; i<content.size(); i++){
		QChar c = content[i];
		if(c == '{'){
			quote_counter ++;
		}

		else if(c == '}'){
			quote_counter --;
		}

		if(quote_counter == 0){
			if(c == ',' || c == ']'){
				return i;
			}
		}
	}

	return content.size() - 1;
}

int	SoundcloudJsonParser::find_array_end(const QByteArray& content, int start_at){
	int quote_counter = 0;

	for(int i=start_at; i<content.size(); i++){
		QChar c = content[i];
		if(c == '['){
			quote_counter ++;
		}

		else if(c == ']'){
			quote_counter --;
		}

		if(quote_counter == 0 && c == ','){
			return i;
		}
	}

	return content.size() - 1;
}



JsonItem SoundcloudJsonParser::parse(QString key, const QByteArray& content){

	JsonItem ret;

	if(content.startsWith('[')){
		ret = parse_array(key, content);
	}

	else if(content.startsWith('{')){
		ret = parse_block(key, content);
	}

	else {
		ret = parse_standard(key, content);
	}

	return ret;
}

JsonItem SoundcloudJsonParser::parse_array(QString key, QByteArray content){

	JsonItem ret;
	ret.key = key;
	ret.type = JsonItem::Type::Array;

	remove_first_and_last(content, '[', ']');

	int i=0;
	forever{

		int end = find_block_end(content);
		if(end == 0) break;

		QByteArray str = content.left(end + 1);
		if(str.size() == 0) break;

		ret.values << parse_block(QString::number(i), str);

		int letters_left = content.size() - str.size();
		if(letters_left <= 0) break;

		content = content.right(letters_left);
		i++;
	}

	return ret;
}

JsonItem SoundcloudJsonParser::parse_block(QString key, QByteArray content){

	JsonItem ret;
	ret.key = key;
	ret.type = JsonItem::Type::Block;

	remove_first_and_last(content, '{', '}');

	forever{

		// find key

		QString item_key;

		int i;
		int quote_counter = 0;
		for(i=0; i<content.size(); i++){

			char c = content[i];

			if(c == '\"'){
				if(i > 0 && content[i-1] == '\\'){
					;
				}

				else{
					quote_counter ++;
				}
			}

			else if(quote_counter == 1){
				item_key.push_back(c);
			}

			if(quote_counter == 2){
				if(c == ':'){
					i++;
					break;
				}
			}
		}

		if(content.size() == 0){
			break;
		}

		if(quote_counter == 0){
			break;
		}

		if(i >= content.size() - 2){
			break;
		}

		char c = content[i];
		QByteArray substr;
		content = content.right(content.size() - i);

		int new_start;

		if(c == '['){
			new_start = find_array_end(content);
			substr = content.left(new_start);
			ret.values << parse_array(item_key, substr);
		}

		else if(c == '{'){
			new_start = find_block_end(content);
			substr = content.left(new_start);
			ret.values << parse_block(item_key, substr);
		}

		else{
			new_start = find_value_end(content);
			substr = content.left(new_start);
			ret.values << parse_standard(item_key, substr);
		}

		content = content.right(content.size() - new_start);
		if(content.size() == 0) break;
	}

	return ret;
}

JsonItem SoundcloudJsonParser::parse_standard(QString key, QByteArray content){

	JsonItem ret;
	ret.key = key;

	int end = find_value_end(content);
	content = content.left(end + 1);

	if(content.startsWith('\"')){
		remove_first_and_last(content, '\"', '\"');
		ret.type = JsonItem::Type::String;
	}

	else{
		ret.type = JsonItem::Type::Number;
	}

	ret.pure_value = content;

	return ret;
}

bool SoundcloudJsonParser::extract_track(const JsonItem& item, MetaData& md){

	if(item.type != JsonItem::Type::Block){
		return false;
	}

	for(const JsonItem& track_info : item.values){

		QString value = track_info.pure_value;
		if(value.trimmed().compare("null", Qt::CaseInsensitive) == 0){
			value = "";
		}

		if(track_info.key == "duration"){
			md.length_ms = (quint32) value.toLong();
		}

		else if(track_info.key == "user"){

			Artist artist;
			extract_artist( track_info, artist);

			md.artist_id = artist.id;
			md.artist = artist.name;
		}

		else if(track_info.key == "title"){
			md.title = value;
		}

		else if(track_info.key == "release_year"){
			md.year = value.toInt();
		}

		else if(track_info.key == "stream_url"){
			md.set_filepath(value + '?' + CLIENT_ID_STR);
		}

		else if(track_info.key == "purchase_url" && !value.isEmpty()){
			md.add_custom_field(track_info.key, tr("Purchase Url"), Helper::create_link(value));
		}

		else if(track_info.key == "id" && !value.isEmpty()){
			md.id = value.toInt();
		}

		else if(track_info.key == "genre"){
			md.genres << value;
		}

		else if(track_info.key == "artwork_url"){
			md.cover_download_url = value;
		}

		else if(track_info.key == "original_content_size"){
			md.filesize = value.toLongLong();
		}
	}

	return true;
}

bool SoundcloudJsonParser::extract_artist(const JsonItem& item, Artist& artist){

	if(item.type != JsonItem::Type::Block){
		return false;
	}

	int followers=0;
	int following=0;

	for(const JsonItem& artist_info : item.values){

		QString value = QString::fromUtf8(artist_info.pure_value);
		value.replace("\\n", "<br />");
		value.replace("\\\"", "\"");

		if(value.trimmed().compare("null", Qt::CaseInsensitive) == 0){
			value = "";
		}

		if(artist_info.key == "username"){
			artist.name = value;
		}

		else if(artist_info.key == "avatar_url"){
			artist.cover_download_url = value;
		}

		else if(artist_info.key == "website" && !value.isEmpty()){
			artist.add_custom_field(artist_info.key, tr("Website"), value);
		}

		else if(artist_info.key == "permalink_url" && !value.isEmpty()){
			artist.add_custom_field(artist_info.key, tr("Permalink Url"), Helper::create_link("Soundcloud", value));
		}

		else if(artist_info.key == "description" && !value.isEmpty()){
			artist.add_custom_field(artist_info.key, tr("About"), value);
		}

		else if(artist_info.key == "id" && !value.isEmpty()){
			artist.id = value.toInt();
		}

		else if(artist_info.key == "followers_count" && !value.isEmpty()){
			value.replace("\"", "");

			followers = value.toInt();
		}

		else if(artist_info.key == "followings_count" && !value.isEmpty()){
			value.replace("\"", "");

			following = value.toInt();
		}
	}

	if(followers != -1 && following != -1){
		artist.add_custom_field("followers_following", tr("Followers/Following"), QString::number(followers) + "/" + QString::number(following));
	}

	return true;
}

bool SoundcloudJsonParser::extract_playlist(const JsonItem& item, Album& album, MetaDataList& v_md){

	if(item.type != JsonItem::Type::Block){
		return false;
	}

	Artist artist;

	// iterate over elements in album
	for(const JsonItem& album_info : item.values){

		QString value = album_info.pure_value;
		value.replace("\\n", "<br />");
		if(value.trimmed().compare("null", Qt::CaseInsensitive) == 0){
			value = "";
		}

		if(album_info.key == "uri"){

			QString uri = value;
			int idx = uri.lastIndexOf("/") + 1;
			if(idx == 0) continue;
		}

		else if(album_info.key == "title"){
			album.name = value;
		}

		else if(album_info.key == "track_count"){
			album.num_songs = value.toInt();
		}

		else if(album_info.key == "duration"){
			album.length_sec = (quint32) (value.toLong() / 1000);
		}

		else if(album_info.key == "artwork_url"){
			album.cover_download_url = value;
		}

		else if(album_info.key == "tracks"){
			int i=1;
			for(const JsonItem& track : album_info.values){

				MetaData md;

				bool success = extract_track(track, md);
				md.track_num = i++;

				if(success){
					v_md << std::move(md);
				}
			}
		}

		else if(album_info.key == "permalink" && !value.isEmpty()){
			album.add_custom_field(album_info.key, tr("Permalink Url"), Helper::create_link("Soundcloud", value));
		}

		else if(album_info.key == "purchase_url" && !value.isEmpty()){
			album.add_custom_field(album_info.key, tr("Purchase Url"), Helper::create_link(value));
		}

		else if(album_info.key == "id" && !value.isEmpty()){
			album.id = value.toInt();
		}

		else if(album_info.key == "user"){
			extract_artist(album_info, artist);
		}
	}

	for(MetaData& md : v_md){

		md.album_id = album.id;
		md.album = album.name;

		if(artist.id > 0){
			md.artist = artist.name;
			md.artist = artist.id;
		}

		if(!album.cover_download_url.isEmpty()){
			md.cover_download_url = album.cover_download_url;
		}
	}

	album.artists << artist.name;

	return true;
}



bool SoundcloudJsonParser::parse_artists(ArtistList& artists){

	artists.clear();
	JsonItem item = parse("Artists", _content);

	if(item.type == JsonItem::Type::Block){
		Artist artist;
		extract_artist(item, artist);
		artists << artist;
		return true;
	}


	for(const JsonItem& artist_item : item.values){
		Artist artist;
		extract_artist(artist_item, artist);
		artists << artist;
	}

	return true;
}


bool SoundcloudJsonParser::parse_tracks(MetaDataList& v_md){

	v_md.clear();
	JsonItem item = parse("Tracks", _content);

	for(const JsonItem& track_item : item.values){
		MetaData md;
		extract_track(track_item, md);
		v_md << std::move(md);
	}

	return true;
}


bool SoundcloudJsonParser::parse_playlists(AlbumList& albums, MetaDataList& v_md){

	albums.clear();

	JsonItem item = parse("Playlists", _content);

	for(const JsonItem& album_item : item.values){

		MetaDataList v_md_tmp;
		Album album_tmp;

		extract_playlist(album_item, album_tmp, v_md_tmp);

		if(v_md_tmp.size() == 0) continue;

		for(MetaData& md : v_md_tmp){
			v_md << std::move(md);
		}

		albums << album_tmp;
	}

	return true;
}
