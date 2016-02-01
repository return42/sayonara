/* MetaData.cpp */

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



#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/MetaDataInfo.h"
#include <QDir>
#include <QUrl>

#include <algorithm>

#define MD_DO_COPY \
	id = other.id; \
	artist_id = other.artist_id; \
	album_id = other.album_id; \
	title = other.title; \
	artist = other.artist; \
	album = other.album; \
	genres = other.genres; \
	rating = other.rating; \
	length_ms = other.length_ms; \
	year = other.year; \
	_filepath = other.filepath(); \
	track_num = other.track_num; \
	bitrate = other.bitrate; \
	is_extern = other.is_extern; \
	_radio_mode = other.radio_mode(); \
	filesize = other.filesize; \
	comment = other.comment; \
	discnumber = other.discnumber; \
	n_discs = other.n_discs; \
	pl_playing = other.pl_playing; \
	is_disabled = other.is_disabled; \
	played = other.played;



MetaData::MetaData() :
	LibraryItem()
{

	id = -1;
	artist_id = -1;
	album_id = -1;
	//title = QString();
	//artist = QString();
	//album = QString();
	rating = 0;
	length_ms = 0;
	year = 0;
	//_filepath = QString();
	track_num = 0;
	bitrate = 0;
	is_extern = false;
	_radio_mode = RadioMode::Off;
	filesize = 0;
	//comment = QString();
	discnumber = 0;
	n_discs = 0;
	//genres = QStringList();

	pl_playing = false;

	is_disabled = false;
	played = false;

}

MetaData::MetaData(const MetaData & other) :
	LibraryItem(other)
{
	MD_DO_COPY
}


MetaData::MetaData(MetaData&& other) :
	LibraryItem(other)
{
	MD_DO_COPY
}

MetaData::~MetaData() {

}

QString MetaData::to_string() const {
	return	title + " by " +
			artist + " from " +
			album +
			//" (" + QString::number(length_ms) + " m_sec) :: " +
			//_filepath +
			"";

}

QVariant MetaData::toVariant(const MetaData& md) {

	QVariant v;

	v.setValue<MetaData>(md);

	return v;
}

bool MetaData::fromVariant(const QVariant& v, MetaData& md) {

	if(! v.canConvert<MetaData>() ) {
		return false;
	}

	md = v.value<MetaData>() ;
	return true;
}

MetaData& MetaData::operator=(const MetaData& other)
{
	LibraryItem::operator=(other);
	MD_DO_COPY
	return *this;
}



bool MetaData::operator==(const MetaData& md) const {
	return this->is_equal(md);
}


bool MetaData::operator!=(const MetaData& md) const {
	return !(this->is_equal(md));
}



bool MetaData::is_equal(const MetaData& md) const {

	QDir first_path(_filepath);
	QDir other_path(md.filepath());

	QString s_first_path = first_path.absolutePath();
	QString s_other_path = other_path.absolutePath();



#ifdef Q_OS_UNIX
	return (s_first_path.compare(s_other_path) == 0);
#else
	return (s_first_path.compare(s_other_path, Qt::CaseInsensitive) == 0);
#endif

}

bool MetaData::is_equal_deep(const MetaData& md) const{

	return ( (id == md.id)  &&
			 ( artist_id == md.artist_id ) &&
			 ( album_id == md.album_id ) &&
			 ( title == md.title ) &&
			 ( artist == md.artist ) &&
			 ( album == md.album ) &&
			 ( genres == md.genres ) &&
			 ( rating == md.rating ) &&
			 ( length_ms == md.length_ms ) &&
			 ( year == md.year ) &&
			 ( filepath() == md.filepath() ) &&
			 ( track_num == md.track_num ) &&
			 ( bitrate == md.bitrate ) &&
			 ( is_extern == md.is_extern ) &&
			 ( _radio_mode == md.radio_mode() ) &&
			 ( filesize == md.filesize ) &&
			 ( comment == md.comment ) &&
			 ( discnumber == md.discnumber ) &&
			 ( n_discs == md.n_discs ) &&

			 ( pl_playing == md.pl_playing ) &&

			 ( is_disabled == md.is_disabled )
			 );
}

QString MetaData::filepath() const{
	return _filepath;
}


QString MetaData::set_filepath(QString filepath){

	bool is_local_path = false;

#ifdef Q_OS_UNIX
	if(filepath.startsWith("/")){
		is_local_path = true;
	}
#else
	if(filepath.contains(":\\") || filepath.contains("\\\\")){
		is_local_path = true;
	}
#endif

	if(is_local_path){
		QDir dir(filepath);
		_filepath = dir.absolutePath();
		_radio_mode = RadioMode::Off;
	}

	else if(filepath.contains("soundcloud.com")){
		_filepath = filepath;
		_radio_mode = RadioMode::Soundcloud;
	}

	else{
		_filepath = filepath;
		_radio_mode = RadioMode::Station;
	}

	return _filepath;
}

RadioMode MetaData::radio_mode() const {
	return _radio_mode;
}


MetaDataList::MetaDataList() : 
	QVector<MetaData>()
{
	_cur_played_track = -1;
}

MetaDataList::MetaDataList(int n_elems) : 
	QVector<MetaData>(n_elems)
{
	_cur_played_track = -1;
}

MetaDataList::MetaDataList(const MetaDataList& lst) :
	QVector(lst)
{
	_cur_played_track = lst._cur_played_track;
}

MetaDataList::~MetaDataList() {

}

void MetaDataList::set_cur_play_track_idx_only(int idx) {
	_cur_played_track = idx;
}

void MetaDataList::set_cur_play_track(int idx) {

	_cur_played_track = -1;
	if(idx < 0 || idx >= this->size()) {
		return;
	}

	int tmp_idx=0;
	for(auto it=this->begin(); it != this->end(); it++ ){
		it->pl_playing = (idx == tmp_idx);
		tmp_idx++;
	}

	_cur_played_track = idx;
}

MetaDataList& MetaDataList::insert_tracks(const MetaDataList& v_md, int tgt_idx){

	if(v_md.size() == 0) {
		return *this;
	}

	int end_point;
	int cur_track;

	tgt_idx = std::max(0, tgt_idx);
	tgt_idx = std::min(this->size(), tgt_idx);

	end_point = this->size();
	cur_track = this->get_cur_play_track();

	this->resize(this->size() + v_md.size());

	std::move_backward( this->begin() + tgt_idx, this->begin() + end_point, this->end());

	for(const MetaData& md : v_md) {
		this->operator[](tgt_idx) = std::move(md);
		this->operator[](tgt_idx).is_disabled = !(Helper::check_track(md));

		tgt_idx++;
	}

	if(cur_track >= tgt_idx){
		_cur_played_track = cur_track;
	}

	return *this;
}

MetaDataList& MetaDataList::move_tracks(const IdxList& rows, int tgt_idx){

	MetaDataList v_md_to_move(rows.size());
	MetaDataList v_md_before_tgt;
	MetaDataList v_md_after_tgt;

	int cur_track[3] = {-1,-1,-1};
	int i=0;
	int idx_to_move=0;
	for(auto it=this->begin(); it!=this->end(); it++, i++) {

		const MetaData& md = *it;

		if(!rows.contains(i) && i < tgt_idx){
			v_md_before_tgt << std::move( md );
			if(md.pl_playing){
				cur_track[0] = v_md_before_tgt.size() - 1;
			}
		}

		else if(!rows.contains(i) && i >= tgt_idx){
			v_md_after_tgt << std::move( md );
			if(md.pl_playing){
				cur_track[2] = v_md_after_tgt.size() - 1;
			}
		}

		else if(rows.contains(i)){
			v_md_to_move[idx_to_move] = std::move( md );
			if(md.pl_playing){
				cur_track[1] = v_md_after_tgt.size() - 1;
			}

			idx_to_move++;
		}
	}

	cur_track[1] += v_md_before_tgt.size();
	cur_track[2] += v_md_before_tgt.size() + v_md_to_move.size();

	int start_idx = 0;

	std::move(v_md_before_tgt.begin(), v_md_before_tgt.end(), this->begin());

	start_idx += v_md_before_tgt.size();

	std::move(v_md_to_move.begin(), v_md_to_move.end(), this->begin() + start_idx);

	start_idx += v_md_to_move.size();

	std::move(v_md_after_tgt.begin(), v_md_after_tgt.end(), this->begin() + start_idx);

	for(int i=0; i<2; i++){
		if(cur_track[i] >= 0){
			_cur_played_track = cur_track[i];
			break;
		}
	}

	return *this;
}

MetaDataList& MetaDataList::randomize(){
	RandomGenerator generator;

	for(auto it=this->begin(); it != this->end(); it++){
		int rnd_number = generator.get_number(0, this->size()-1);
		std::swap(*it, *(this->begin() + rnd_number));
	}

	return *this;
}

MetaDataList MetaDataList::extract_tracks(std::function<bool (const MetaData &)> func) const
{
	if(this->isEmpty()){
		return MetaDataList();
	}

	MetaDataList v_md;
	auto it = this->begin();

	while(it != this->end()){

		it = std::find_if(it, this->end(), func);
		if(it == this->end()){
			break;
		}

		else{
			v_md << *it;
			it++;
		}
	}

	return v_md;
}

MetaDataList MetaDataList::extract_tracks(const IdxList& idx_list) const
{
	MetaDataList v_md;
	for(int idx : idx_list){
		v_md << this->operator [](idx);
	}
	return v_md;
}

MetaDataList& MetaDataList::remove_track(int idx){
	if(!between(idx, 0, this->size())){
		return *this;
	}

	for(auto it=this->begin() + idx; it!=this->end(); it++){

		auto it_next = it + 1;
		*it = std::move(*it_next);
	}

	this->removeLast();
	return *this;
}

MetaDataList& MetaDataList::remove_tracks(IdxList rows){

	if(this->isEmpty() || rows.isEmpty()){
		return *this;
	}

	// first, sort the idxs list
	auto lambda = [](int i, int j) -> bool
	{
		return i<j;
	};


	std::sort(rows.begin(), rows.end(), lambda);

	int row_writing_idx=0;
	int deleted_rows=0;

	auto it_idx = rows.begin();

	for(int i=0; i<this->size(); i++){

		if(i == *it_idx && it_idx != rows.end()){

			deleted_rows++;
			it_idx++;
		}

		else{

			if(i != row_writing_idx){
				this->operator [](row_writing_idx) = std::move(this->operator [](i));
			}

			row_writing_idx++;
		}
	}
	this->resize(this->size() - deleted_rows);
	return *this;
}

int MetaDataList::get_cur_play_track() const {
	return _cur_played_track;
}


bool MetaDataList::contains(const MetaData& md) const {

	for(auto it = this->begin(); it != this->end(); it++) {

		if( it->is_equal(md) ) {
			return true;
		}
	}

	return false;
}

IdxList MetaDataList::findTracks(int id) const {

	IdxList ret;
	int idx=0;

	if(id == -1) {
		return ret;
	}

	auto lambda = [&id, &idx, &ret](const MetaData& md) {
		if(md.id == id){
			ret << idx;
		}

		idx++;
	};

	std::for_each(this->begin(), this->end(), lambda);

	return ret;
}

IdxList MetaDataList::findTracks(const QString& path) const {

	IdxList ret;
	int idx=0;

#ifdef Q_OS_UNIX
	Qt::CaseSensitivity sensitivity = Qt::CaseSensitive;
#else
	Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive;
#endif

	auto lambda = [&ret, &idx, &path, &sensitivity](const MetaData& md){
		if(md.filepath().compare(path, sensitivity) == 0){
			ret << idx;
		}

		idx++;
	};

	std::for_each(this->begin(), this->end(), lambda);

	return ret;
}


QStringList MetaDataList::toStringList() const {

	QStringList lst;

	auto lambda = [&lst](const MetaData& md){
		if(md.id >= 0){
			lst << QString::number(md.id);
		}
		else{
			lst << md.filepath();
		}
	};

	for_each(this->begin(), this->end(), lambda);

	return lst;
}

MetaDataInfo* MetaDataList::get_info() const {
	MetaDataInfo* info = new MetaDataInfo(this);
	return info;
}

AlbumInfo* MetaDataList::get_album_info() const {
	AlbumInfo* info = new AlbumInfo(this);
	return info;
}

ArtistInfo* MetaDataList::get_artist_info() const {
	ArtistInfo* info = new ArtistInfo(this);
	return info;
}
