/* MetaDataList.cpp */

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



#include "MetaDataList.h"
#include "Helper/FileHelper.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Random/RandomGenerator.h"
#include "Helper/Set.h"

#include <algorithm>

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
	if( !between(idx, 0, this->size())) {
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
	int old_tgt_idx = tgt_idx;

	tgt_idx = std::max(0, tgt_idx);
	tgt_idx = std::min(this->size(), tgt_idx);

	end_point = this->size();
	cur_track = this->get_cur_play_track();

	this->resize(this->size() + v_md.size());

	std::move_backward( this->begin() + tgt_idx, this->begin() + end_point, this->end());

	for(const MetaData& md : v_md) {
		QString path = md.filepath();
		this->operator[](tgt_idx) = std::move(md);
		this->operator[](tgt_idx).is_disabled = !(Helper::File::check_file(path));

		tgt_idx++;
	}

	if(cur_track >= old_tgt_idx){
		_cur_played_track = cur_track + v_md.size();
	}

	return *this;
}

MetaDataList& MetaDataList::copy_tracks(const SP::Set<int>& indexes, int tgt_idx){

	MetaDataList v_md;
	for(int idx : indexes){
		v_md << this->operator[](idx);
	}

	return insert_tracks(v_md, tgt_idx);
}


MetaDataList& MetaDataList::move_tracks(const SP::Set<int>& indexes, int tgt_idx){

	MetaDataList v_md_to_move(indexes.size());
	MetaDataList v_md_before_tgt;
	MetaDataList v_md_after_tgt;

	int cur_track[3] = {-1,-1,-1};
	int i=0;
	int idx_to_move=0;
	for(auto it=this->begin(); it!=this->end(); it++, i++) {

		const MetaData& md = *it;

		bool contains_i = indexes.contains(i);

		if(!contains_i && i < tgt_idx){
			v_md_before_tgt << std::move( md );
			if(md.pl_playing){
				cur_track[0] = v_md_before_tgt.size() - 1;
			}
		}

		else if(!contains_i && i >= tgt_idx){
			v_md_after_tgt << std::move( md );
			if(md.pl_playing){
				cur_track[2] = v_md_after_tgt.size() - 1;
			}
		}

		else if(contains_i){
			v_md_to_move[idx_to_move] = std::move( md );
			if(md.pl_playing){
				cur_track[1] = v_md_to_move.size() - 1;
			}

			idx_to_move++;
		}
	}

	if( cur_track[1] >= 0 ){
		cur_track[1] += v_md_before_tgt.size();
	}

	if( cur_track[2] >= 0 ){
		cur_track[2] += v_md_before_tgt.size() + v_md_to_move.size();
	}

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

MetaDataList MetaDataList::extract_tracks(const SP::Set<int>& indexes) const
{
	MetaDataList v_md;

	std::for_each(indexes.begin(), indexes.end(), [this, &v_md](int row){
		v_md << this->operator[](row);
	});

	return v_md;
}


MetaDataList& MetaDataList::remove_track(int idx){
	return remove_tracks(idx, idx);
}

MetaDataList& MetaDataList::remove_tracks(int first, int last){
	if(!between(first, 0, this->size())){
		return *this;
	}

	if(!between(last, 0, this->size())){
		return *this;
	}

	std::move(this->begin() + last + 1, this->end(), this->begin() + first);

	if(_cur_played_track >= first && _cur_played_track <= last){
		_cur_played_track = -1;
	}

	if(_cur_played_track > last){
		_cur_played_track -= (last - first + 1);
	}

	this->resize(this->size() - (last - first + 1));
	return *this;

}

MetaDataList& MetaDataList::remove_tracks(const SP::Set<int>& indexes){

	move_tracks(indexes, this->size() - 1);
	this->resize(this->size() - indexes.size());
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

	std::for_each(this->begin(), this->end(), lambda);

	return lst;
}


MetaDataList& MetaDataList::operator <<(const MetaDataList& v_md)
{
	int last_idx = this->size();
	this->resize(this->size() + v_md.size());


	for(const MetaData& md : v_md){
		this->operator [](last_idx) = md;
		last_idx++;
	}

	return *this;
}


MetaDataList& MetaDataList::operator <<(const MetaData& md)
{
	this->push_back(md);
	return *this;
}

bool MetaDataList::contains(qint32 id) const
{
	for(auto it=this->begin(); it!=this->end(); it++){
		if(it->id == id){
			return true;
		}
	}

	return false;
}
