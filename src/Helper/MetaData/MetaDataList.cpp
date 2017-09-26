/* MetaDataList.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
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
#include "Helper/Random/RandomGenerator.h"
#include "Helper/Set.h"
#include "Helper/globals.h"

#include <algorithm>

struct MetaDataList::Private
{
	int current_track;
	Private()
	{
		current_track = -1;
	}
};

MetaDataList::MetaDataList() :
	std::vector<MetaData>()
{
	_m = Pimpl::make<Private>();
}

MetaDataList::MetaDataList(const MetaDataList& other) :
	std::vector<MetaData>(other)
{
	_m = Pimpl::make<Private>();
	_m->current_track = other.current_track();
}

MetaDataList::~MetaDataList() {}

MetaDataList& MetaDataList::operator=(const MetaDataList& other)
{
	this->resize(other.count());
	std::copy(other.begin(), other.end(), this->begin());

	_m->current_track = other.current_track();

	return *this;
}

int MetaDataList::current_track() const
{
	return _m->current_track;
}

void MetaDataList::set_current_track(int idx)
{
	_m->current_track = -1;

	if( !between(idx, this)) {
		return;
	}

	int tmp_idx=0;
	for(auto it=this->begin(); it != this->end(); it++ ){
		it->pl_playing = (idx == tmp_idx);
		tmp_idx++;
	}

	_m->current_track = idx;
}

MetaDataList& MetaDataList::insert_track(const MetaData& md, int tgt_idx)
{
	MetaDataList v_md; v_md << md;
	return insert_tracks(v_md, tgt_idx);
}

MetaDataList& MetaDataList::insert_tracks(const MetaDataList& v_md, int tgt_idx)
{
	if(v_md.isEmpty()) {
		return *this;
	}

	tgt_idx = std::max(0, tgt_idx);
	tgt_idx = std::min((int) this->count(), tgt_idx);

	this->resize(this->count() + v_md.count());

	std::move(this->begin() + tgt_idx, this->end(), this->begin() + v_md.count());
	std::copy(v_md.begin(), v_md.end(), this->begin() + tgt_idx);

	if(current_track() >= tgt_idx){
		set_current_track(current_track() + v_md.count());
	}

	return *this;
}

MetaDataList& MetaDataList::copy_tracks(const SP::Set<int>& indexes, int tgt_idx)
{
	MetaDataList v_md; v_md.reserve(indexes.size());

	for(int idx : indexes){
		v_md << this->operator[](idx);
	}

	return insert_tracks(v_md, tgt_idx);
}


MetaDataList& MetaDataList::move_tracks(const SP::Set<int>& indexes, int tgt_idx)
{
	MetaDataList v_md_to_move; 		v_md_to_move.reserve(indexes.size());
	MetaDataList v_md_before_tgt; 	v_md_before_tgt.reserve(count() + indexes.size());
	MetaDataList v_md_after_tgt; 	v_md_after_tgt.reserve(count() + indexes.size());

	int i=0;
	int n_tracks_after_cur_idx = 0;
	int n_tracks_before_cur_idx = 0;

	bool contains_cur_track = false;

	for(auto it=this->begin(); it!=this->end(); it++, i++)
	{
		const MetaData& md = *it;

		it->pl_playing = (i == _m->current_track);

		bool contains_i = indexes.contains(i);

		if(!contains_i) {
			if(i<tgt_idx) {
				v_md_before_tgt << std::move( md );
			} else {
				v_md_after_tgt << std::move( md );
			}
		} else {
			contains_cur_track |= (i == _m->current_track);
			v_md_to_move << std::move( md );
			if(i < _m->current_track){
				n_tracks_before_cur_idx++;
			}

			else if(i > _m->current_track){
				n_tracks_after_cur_idx++;
			}
		}
	}

	int start_idx = 0;

	std::move(v_md_before_tgt.begin(), v_md_before_tgt.end(), this->begin());
	start_idx += v_md_before_tgt.count();

	std::move(v_md_to_move.begin(), v_md_to_move.end(), this->begin() + start_idx);
	start_idx += v_md_to_move.count();

	std::move(v_md_after_tgt.begin(), v_md_after_tgt.end(), this->begin() + start_idx);

	if(contains_cur_track) {
		_m->current_track = (n_tracks_before_cur_idx) + v_md_before_tgt.count();
	}

	if(!contains_cur_track) 
	{
		if(tgt_idx <= _m->current_track) {
			_m->current_track += n_tracks_after_cur_idx;
		}

		else {
			_m->current_track -= n_tracks_before_cur_idx;
		}
	}

	return *this;
}

MetaDataList& MetaDataList::remove_track(int idx)
{
	return remove_tracks(idx, idx);
}

MetaDataList& MetaDataList::remove_tracks(int first, int last)
{
	if( !between(first, this) || 
		!between(last, this))
	{
		return *this;
	}

	int n_elems = last - first + 1;

	if(last != this->count() - 1) {
		std::move(this->begin() + last + 1, this->end(), this->begin()  + first);
	}

	this->resize(this->count() - n_elems);

	if(_m->current_track >= first && _m->current_track <= last){
		set_current_track(-1);
	}

	if(_m->current_track > last){
		set_current_track( _m->current_track - n_elems );
	}

	return *this;
}

MetaDataList& MetaDataList::remove_tracks(const SP::Set<int>& indexes)
{
	int deleted_elements = 0;
	for(int i : indexes)
	{
		std::move(
				this->begin() + (i - deleted_elements + 1), 
				this->end(), 
				this->begin() + (i - deleted_elements)
		);

		deleted_elements++;
	}

	this->resize(this->count() - deleted_elements);

	if(indexes.contains(_m->current_track))
	{
		_m->current_track = -1;
	}

	else
	{
		int n_tracks_before_cur_track = std::count_if(indexes.begin(), indexes.end(), [=](int idx){
			return (idx < _m->current_track);
		});

		_m->current_track -= n_tracks_before_cur_track;
	}

	return *this;
}


bool MetaDataList::contains(const MetaData& md) const
{
	auto it = std::find_if(this->begin(), this->end(), [&md](const MetaData& md_tmp){
		return md.is_equal(md_tmp);
	});

	return (it != this->end());
}

QList<int> MetaDataList::findTracks(int id) const
{
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

QList<int> MetaDataList::findTracks(const QString& path) const
{
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


QStringList MetaDataList::toStringList() const
{
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
    int old_count = this->count();
	this->resize(this->count() + v_md.count());
    auto it = this->begin() + old_count;

	std::copy(v_md.begin(), v_md.end(), it);

	return *this;
}


MetaDataList& MetaDataList::operator <<(const MetaData& md)
{
	this->push_back(md);
	return *this;
}

bool MetaDataList::contains(int32_t id) const
{
	auto it = std::find_if(this->begin(), this->end(), [&id](const MetaData& md){
		return (id == md.id);
	});

	return (it != this->end());
}

void MetaDataList::remove_duplicates()
{
	for(auto it=this->begin(); it!=this->end(); it++){
		auto it_next = it + 1;

		if(it_next == this->end()){
			break;
		}

		auto last_it2 = it;
		for(auto it2=it_next; it2 != this->end(); it2++) {

			if(it->filepath().compare(it2->filepath()) == 0){
				auto it2_next = it2 + 1;
				if(it2_next != this->end()){
					std::move(it2_next, this->end(), it2);
				}

				this->resize(this->count() - 1);

				it2 = last_it2 + 1;
				if(it2 == this->end()){
					break;
				}
			}

			else {
				last_it2 = it2;
			}
		}
	}
}

bool MetaDataList::isEmpty() const
{
	return this->empty();
}

const MetaData& MetaDataList::first() const
{
	return at(0);
}

const MetaData& MetaDataList::last() const
{
	return at(this->count() - 1);
}

int MetaDataList::count() const
{
	return (int) std::vector<MetaData>::size();
}

MetaData MetaDataList::take_at(int idx)
{
	MetaData md = this->at(idx);
	this->remove_track(idx);
	return md;
}
