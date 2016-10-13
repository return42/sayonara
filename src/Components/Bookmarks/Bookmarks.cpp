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

#include "Bookmark.h"
#include "Bookmarks.h"
#include "Helper/Helper.h"
#include "Helper/globals.h"
#include "Helper/MetaData/MetaData.h"
#include "Database/DatabaseConnector.h"
#include "Components/PlayManager/PlayManager.h"

#include <algorithm>

struct Bookmarks::Private
{
	DatabaseConnector*		db=nullptr;
	PlayManager*			play_manager=nullptr;

	QList<Bookmark>			bookmarks;
	MetaData				md;

	quint32					cur_time;
	int						prev_idx;
	int						next_idx;

	quint32					loop_start;
	quint32					loop_end;
};

Bookmarks::Bookmarks(QObject *parent) :
	QObject(parent)
{
	_m = Pimpl::make<Bookmarks::Private>();

	_m->play_manager = PlayManager::getInstance();
	_m->db = DatabaseConnector::getInstance();

	init_members();

	connect(_m->play_manager, &PlayManager::sig_track_changed, this, &Bookmarks::track_changed);
	connect(_m->play_manager, &PlayManager::sig_position_changed_ms, this, &Bookmarks::pos_changed_ms);
	connect(_m->play_manager, &PlayManager::sig_playstate_changed,	this, &Bookmarks::playstate_changed);

	reload_bookmarks();
}

Bookmarks::~Bookmarks() {}

void Bookmarks::init_members()
{
	_m->bookmarks.clear();
	_m->cur_time = 0;
	_m->prev_idx = -1;
	_m->next_idx = -1;
	_m->loop_start = 0;
	_m->loop_end = 0;

	_m->md = _m->play_manager->get_cur_track();
}


void Bookmarks::sort_bookmarks(){

	auto lambda = [](const Bookmark& bm1, const Bookmark& bm2){
		return bm1.get_time() < bm2.get_time();
	};

	std::sort(_m->bookmarks.begin(), _m->bookmarks.end(), lambda);
}

void Bookmarks::reload_bookmarks(){

	QMap<quint32, QString> bookmarks;
	if(_m->md.id >= 0){
		_m->db->searchBookmarks(_m->md.id, bookmarks);
	}

	_m->bookmarks.clear();
	for(quint32 key : bookmarks.keys()){
		_m->bookmarks << Bookmark(key, bookmarks[key], true);
	}

	sort_bookmarks();

	emit sig_bookmarks_changed(_m->bookmarks);
}


bool Bookmarks::save()
{
	quint32 cur_time = _m->cur_time;
	if(cur_time == 0) {
		return false;
	}

	bool already_there = std::any_of(_m->bookmarks.begin(), _m->bookmarks.end(), [&cur_time](const Bookmark& bm){
		return (bm.get_time() == cur_time);
	});

	if(already_there){
		return true;
	}

	QString name = Helper::cvt_ms_to_string(cur_time * 1000, true, true, false);
	bool success = _m->db->insertBookmark(_m->md.id, cur_time, name);

	if(success){
		reload_bookmarks();
	}

	return success;
}


bool Bookmarks::remove(int idx){
	bool success = _m->db->removeBookmark(_m->md.id, _m->bookmarks[idx].get_time());

	if(success){
		reload_bookmarks();
	}

	return success;
}


bool Bookmarks::jump_to(int idx){
	_m->play_manager->seek_abs_ms(_m->bookmarks[idx].get_time() * 1000);
	return true;
}

bool Bookmarks::jump_next(){
	if( !between(_m->next_idx, _m->bookmarks) ){
		emit sig_next_changed(Bookmark());
		return false;
	}

	quint32 new_time = _m->bookmarks[_m->next_idx].get_time();
	_m->play_manager->seek_abs_ms(new_time * 1000);

	return true;
}

bool Bookmarks::jump_prev() {

	quint32 new_time;

	if( _m->prev_idx >= _m->bookmarks.size() ){
		emit sig_prev_changed(Bookmark());
		return false;
	}

	if(_m->prev_idx < 0){
		new_time = 0;
	}
	else{
		new_time = _m->bookmarks[_m->prev_idx].get_time();
	}

	_m->play_manager->seek_abs_ms(new_time * 1000);

	return true;
}


void Bookmarks::pos_changed_ms(quint64 pos_ms){

	_m->cur_time = (quint32) (pos_ms / 1000);

	if( _m->cur_time >= _m->loop_end &&
		_m->loop_end != 0)
	{
		jump_prev();
		return;
	}

	if(_m->bookmarks.isEmpty()){
		return;
	}

	_m->prev_idx=-1;
	_m->next_idx=-1;


	int i=0;
	for(Bookmark& bookmark : _m->bookmarks){

		quint32 time = bookmark.get_time();

		if(time < _m->cur_time){
			_m->prev_idx = i;
		}

		else if(time > _m->cur_time){
			if(_m->next_idx == -1){
				_m->next_idx = i;
				break;
			}
		}

		i++;
	}

	if( between(_m->prev_idx, _m->bookmarks) ){
		emit sig_prev_changed(_m->bookmarks[_m->prev_idx]);
	}
	else{
		emit sig_prev_changed(Bookmark());
	}

	if( between(_m->next_idx, _m->bookmarks) ){
		emit sig_next_changed(_m->bookmarks[_m->next_idx]);
	}
	else{
		emit sig_next_changed(Bookmark());
	}
}


void Bookmarks::track_changed(const MetaData& md)
{
	_m->md = md;
	_m->loop_start = 0;
	_m->loop_end = 0;

	if(!_m->md.get_custom_field("Chapter1").isEmpty())
	{
		int chapter_idx = 1;
		QString entry;
		_m->bookmarks.clear();

		do{
			QString custom_field_name = QString("Chapter") + QString::number(chapter_idx);

			entry = _m->md.get_custom_field(custom_field_name);

			QStringList lst = entry.split(":");
			quint32 length = lst.takeFirst().toInt();
			QString name = lst.join(":");

			_m->bookmarks << Bookmark(length, name, true);
			chapter_idx++;

		} while( !entry.isEmpty() );

	}

	else if(md.id < 0){
		init_members();
	}

	else {
		QMap<quint32, QString> bookmarks;
		_m->db->searchBookmarks(md.id, bookmarks);

		_m->bookmarks.clear();
		for(quint32 key : bookmarks.keys()){
			_m->bookmarks << Bookmark(key, bookmarks[key], true);
		}
	}

	sort_bookmarks();

	emit sig_bookmarks_changed(_m->bookmarks);
	emit sig_prev_changed(Bookmark());
	emit sig_next_changed(Bookmark());
}


void Bookmarks::playstate_changed(PlayState state){

	if(state == PlayState::Stopped){
		init_members();
		emit sig_bookmarks_changed(_m->bookmarks);
		emit sig_prev_changed(Bookmark());
		emit sig_next_changed(Bookmark());
	}
}


bool Bookmarks::set_loop(bool b){

	bool ret = false;

	_m->loop_start = 0;
	_m->loop_end = 0;

	if(b){
		if( between(_m->prev_idx, _m->bookmarks) &&
			between(_m->next_idx, _m->bookmarks) )
		{
			_m->loop_start = _m->bookmarks[_m->prev_idx].get_time();
			_m->loop_end = _m->bookmarks[_m->next_idx].get_time();
			ret = true;
		}
	}

	return ret;
}

MetaData Bookmarks::get_cur_track() const
{
	return _m->md;
}

QList<Bookmark> Bookmarks::get_all_bookmarks() const
{
	return _m->bookmarks;
}

int Bookmarks::get_size() const
{
	return _m->bookmarks.size();
}
