/* Bookmarks.cpp */

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



#include "Bookmarks.h"
#include "Database/DatabaseConnector.h"

#include <algorithm> // std::max

Bookmarks::Bookmarks(QObject *parent) :
	QObject(parent)
{

	_play_manager = PlayManager::getInstance();
	_db = DatabaseConnector::getInstance();

	init_members();

	connect(_play_manager, &PlayManager::sig_track_changed, this, &Bookmarks::track_changed);
	connect(_play_manager, &PlayManager::sig_position_changed_ms, this, &Bookmarks::pos_changed_ms);
	connect(_play_manager, &PlayManager::sig_playstate_changed,	this, &Bookmarks::playstate_changed);

	reload_bookmarks();
}

Bookmarks::~Bookmarks()
{

}

void Bookmarks::init_members()
{
	_bookmarks.clear();
	_cur_time = 0;
	_prev_idx = -1;
	_next_idx = -1;
	_loop_start = 0;
	_loop_end = 0;

	_md = _play_manager->get_cur_track();
}


void Bookmarks::sort_bookmarks(){

	auto lambda = [](const Bookmark& bm1, const Bookmark& bm2){
		return bm1.get_time() < bm2.get_time();
	};

	std::sort(_bookmarks.begin(), _bookmarks.end(), lambda);
}

void Bookmarks::reload_bookmarks(){

	QMap<quint32, QString> bookmarks;
	if(_md.id >= 0){
		_db->searchBookmarks(_md.id, bookmarks);
	}

	_bookmarks.clear();
	for(quint32 key : bookmarks.keys()){
		_bookmarks << Bookmark(key, bookmarks[key], true);
	}

	sort_bookmarks();

	emit sig_bookmarks_changed(_bookmarks);
}


bool Bookmarks::save()
{
	if(_cur_time == 0) {
		return false;
	}

	quint32 cur_time = _cur_time;
	bool already_there = std::any_of(_bookmarks.begin(), _bookmarks.end(), [&cur_time](const Bookmark& bm){
		return (bm.get_time() == cur_time);
	});

	if(already_there){
		return true;
	}

	QString name = Helper::cvt_ms_to_string(_cur_time * 1000, true, true, false);
	bool success = _db->insertBookmark(_md.id, _cur_time, name);

	if(success){
		reload_bookmarks();
	}

	return success;
}


bool Bookmarks::remove(int idx){
	bool success = _db->removeBookmark(_md.id, _bookmarks[idx].get_time());

	if(success){
		reload_bookmarks();
	}

	return success;
}


bool Bookmarks::remove_all(){
	bool success = _db->removeAllBookmarks(_md.id);

	if(success){
		reload_bookmarks();
	}

	return success;
}


bool Bookmarks::jump_to(int idx){
	_play_manager->seek_abs_ms(_bookmarks[idx].get_time() * 1000);
	return true;
}

bool Bookmarks::jump_next(){
	if( !between(_next_idx, 0, _bookmarks.size()) ){
		emit sig_next_changed(Bookmark());
		return false;
	}

	quint32 new_time = _bookmarks[_next_idx].get_time();
	_play_manager->seek_abs_ms(new_time * 1000);

	return true;
}

bool Bookmarks::jump_prev() {

	quint32 new_time;

	if( _prev_idx >= _bookmarks.size() ){
		emit sig_prev_changed(Bookmark());
		return false;
	}

	if(_prev_idx < 0){
		new_time = 0;
	}
	else{
		new_time = _bookmarks[_prev_idx].get_time();
	}

	_play_manager->seek_abs_ms(new_time * 1000);

	return true;
}


void Bookmarks::pos_changed_ms(quint64 pos_ms){

	_cur_time = (quint32) (pos_ms / 1000);

	if( _cur_time >= _loop_end &&
		_loop_end != 0)
	{
		jump_prev();
		return;
	}

	if(_bookmarks.isEmpty()){
		return;
	}

	_prev_idx=-1;
	_next_idx=-1;


	int i=0;
	for(Bookmark& bookmark : _bookmarks){

		quint32 time = bookmark.get_time();

		if(time < _cur_time){
			_prev_idx = i;
		}

		else if(time > _cur_time){
			if(_next_idx == -1){
				_next_idx = i;
				break;
			}
		}

		i++;
	}

	if( between(_prev_idx, 0, _bookmarks.size()) ){
		emit sig_prev_changed(_bookmarks[_prev_idx]);
	}
	else{
		emit sig_prev_changed(Bookmark());
	}

	if( between(_next_idx, 0, _bookmarks.size()) ){
		emit sig_next_changed(_bookmarks[_next_idx]);
	}
	else{
		emit sig_next_changed(Bookmark());
	}
}


void Bookmarks::track_changed(const MetaData& md)
{
	_md = md;
	_loop_start = 0;
	_loop_end = 0;

	if(md.id < 0){
		init_members();
	}

	else {
		QMap<quint32, QString> bookmarks;
		_db->searchBookmarks(md.id, bookmarks);

		_bookmarks.clear();
		for(quint32 key : bookmarks.keys()){
			_bookmarks << Bookmark(key, bookmarks[key], true);
		}

		sort_bookmarks();
	}

	emit sig_bookmarks_changed(_bookmarks);
	emit sig_prev_changed(Bookmark());
	emit sig_next_changed(Bookmark());
}


void Bookmarks::playstate_changed(PlayManager::PlayState state){

	if(state == PlayManager::PlayState::Stopped){
		init_members();
		emit sig_bookmarks_changed(_bookmarks);
		emit sig_prev_changed(Bookmark());
		emit sig_next_changed(Bookmark());
	}
}


bool Bookmarks::set_loop(bool b){

	bool ret = false;

	_loop_start = 0;
	_loop_end = 0;

	if(b){
		if( between(_prev_idx, 0, _bookmarks.size()) &&
			between(_next_idx, 0, _bookmarks.size()) )
		{
			_loop_start = _bookmarks[_prev_idx].get_time();
			_loop_end = _bookmarks[_next_idx].get_time();
			ret = true;
		}
	}

	return ret;
}

MetaData Bookmarks::get_cur_track() const
{
	return _md;
}

QList<Bookmark> Bookmarks::get_all_bookmarks() const
{
	return _bookmarks;
}

