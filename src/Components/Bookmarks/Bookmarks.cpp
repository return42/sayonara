/* Bookmarks.cpp */

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

#include "Bookmark.h"
#include "Bookmarks.h"
#include "Utils/Utils.h"
#include "Utils/globals.h"
#include "Utils/MetaData/MetaData.h"
#include "Database/DatabaseConnector.h"
#include "Components/PlayManager/PlayManager.h"

#include <algorithm>
#include <QStringList>

struct Bookmarks::Private
{
	DatabaseConnector*		db=nullptr;
	PlayManagerPtr			play_manager=nullptr;

	QList<Bookmark>			bookmarks;
	MetaData				md;

	int						prev_idx;
	int						next_idx;

	uint32_t				cur_time;

	uint32_t				loop_start;
	uint32_t				loop_end;
};


Bookmarks::Bookmarks(QObject *parent) :
	QObject(parent)
{
	m = Pimpl::make<Bookmarks::Private>();

	m->play_manager = PlayManager::instance();
	m->db = DatabaseConnector::instance();
	m->md = m->play_manager->current_track();

	init_members();

	connect(m->play_manager, &PlayManager::sig_track_changed, this, &Bookmarks::track_changed);
	connect(m->play_manager, &PlayManager::sig_position_changed_ms, this, &Bookmarks::pos_changed_ms);
	connect(m->play_manager, &PlayManager::sig_playstate_changed,	this, &Bookmarks::playstate_changed);

	m->md = m->play_manager->current_track();
	reload_bookmarks();
}

Bookmarks::~Bookmarks() {}

void Bookmarks::init_members()
{
	m->bookmarks.clear();
	m->cur_time = 0;
	m->prev_idx = -1;
	m->next_idx = -1;
	m->loop_start = 0;
	m->loop_end = 0;

	m->md = m->play_manager->current_track();
}


void Bookmarks::sort_bookmarks()
{
	auto lambda = [](const Bookmark& bm1, const Bookmark& bm2){
		return bm1.get_time() < bm2.get_time();
	};

	std::sort(m->bookmarks.begin(), m->bookmarks.end(), lambda);
}

void Bookmarks::reload_bookmarks()
{
	QMap<uint32_t, QString> bookmarks;
	if(m->md.id >= 0){
		m->db->searchBookmarks(m->md.id, bookmarks);
	}

	m->bookmarks.clear();
	for(uint32_t key : bookmarks.keys()){
		m->bookmarks << Bookmark(key, bookmarks[key], true);
	}

	sort_bookmarks();

	emit sig_bookmarks_changed();
}


Bookmarks::CreationStatus Bookmarks::create()
{
	if(m->md.id < 0 || m->md.db_id() != 0)
	{
		return Bookmarks::CreationStatus::NoDBTrack;
	}

	uint32_t cur_time = m->cur_time;
	if(cur_time == 0) {
		return Bookmarks::CreationStatus::OtherError;
	}

	bool already_there = std::any_of(m->bookmarks.begin(), m->bookmarks.end(), [&cur_time](const Bookmark& bm){
		return (bm.get_time() == cur_time);
	});

	if(already_there){
		return Bookmarks::CreationStatus::AlreadyThere;
	}

	QString name = Util::cvt_ms_to_string(cur_time * 1000, true, true, false);
	bool success = m->db->insertBookmark(m->md.id, cur_time, name);

	if(success){
		reload_bookmarks();
		return Bookmarks::CreationStatus::Success;
	}

	return Bookmarks::CreationStatus::DBError;
}


bool Bookmarks::remove(int idx)
{
	if(idx < 0 || idx >= m->bookmarks.size()){
		return false;
	}

	bool success = m->db->removeBookmark(m->md.id, m->bookmarks[idx].get_time());

	if(success){
		reload_bookmarks();
	}

	return success;
}


bool Bookmarks::jump_to(int idx)
{
	if(!between(idx, m->bookmarks)){
		return false;
	}

	if(idx < 0){
		m->play_manager->seek_abs_ms(0);
	}
	else
	{
		uint64_t new_time = m->bookmarks[idx].get_time() * 1000;
		m->play_manager->seek_abs_ms(new_time);
	}

	return true;
}


bool Bookmarks::jump_next()
{
	if( !between(m->next_idx, m->bookmarks) )
	{
		emit sig_next_changed(Bookmark());
		return false;
	}

	jump_to(m->next_idx);

	return true;
}


bool Bookmarks::jump_prev()
{
	if( m->prev_idx >= m->bookmarks.size() )
	{
		emit sig_prev_changed(Bookmark());
		return false;
	}

	jump_to(m->prev_idx);

	return true;
}


void Bookmarks::pos_changed_ms(uint64_t pos_ms)
{
	m->cur_time = (uint32_t) (pos_ms / 1000);

	if( m->cur_time >= m->loop_end &&
		m->loop_end != 0)
	{
		jump_prev();
		return;
	}

	if(m->bookmarks.isEmpty()){
		return;
	}

	m->prev_idx=-1;
	m->next_idx=-1;


	int i=0;
	for(Bookmark& bookmark : m->bookmarks){
		uint32_t time = bookmark.get_time();

		if(time + 2 < m->cur_time){
			m->prev_idx = i;
		}

		else if(time > m->cur_time){
			if(m->next_idx == -1){
				m->next_idx = i;
				break;
			}
		}

		i++;
	}

	if( between(m->prev_idx, m->bookmarks) ){
		emit sig_prev_changed(m->bookmarks[m->prev_idx]);
	}
	else{
		emit sig_prev_changed(Bookmark());
	}

	if( between(m->next_idx, m->bookmarks) ){
		emit sig_next_changed(m->bookmarks[m->next_idx]);
	}
	else{
		emit sig_next_changed(Bookmark());
	}
}


void Bookmarks::track_changed(const MetaData& md)
{
	m->md = md;
	m->loop_start = 0;
	m->loop_end = 0;

	if(!m->md.get_custom_field("Chapter1").isEmpty())
	{
		int chapter_idx = 1;
		QString entry;
		m->bookmarks.clear();

		do{
			QString custom_field_name = QString("Chapter") + QString::number(chapter_idx);

			entry = m->md.get_custom_field(custom_field_name);

			QStringList lst = entry.split(":");
			uint32_t length = lst.takeFirst().toInt();
			QString name = lst.join(":");

			m->bookmarks << Bookmark(length, name, true);
			chapter_idx++;

		} while( !entry.isEmpty() );

	}

	else if(md.id < 0){
		init_members();
	}

	else {
		QMap<uint32_t, QString> bookmarks;
		m->db->searchBookmarks(md.id, bookmarks);

		m->bookmarks.clear();
		for(uint32_t key : bookmarks.keys()){
			m->bookmarks << Bookmark(key, bookmarks[key], true);
		}
	}

	sort_bookmarks();

	emit sig_bookmarks_changed();
	emit sig_prev_changed(Bookmark());
	emit sig_next_changed(Bookmark());
}


void Bookmarks::playstate_changed(PlayState state)
{
	if(state == PlayState::Stopped){
		init_members();
		emit sig_bookmarks_changed();
		emit sig_prev_changed(Bookmark());
		emit sig_next_changed(Bookmark());
	}
}


bool Bookmarks::set_loop(bool b)
{
	bool ret = false;

	m->loop_start = 0;
	m->loop_end = 0;

	if(b){
		if( between(m->prev_idx, m->bookmarks) &&
			between(m->next_idx, m->bookmarks) )
		{
			m->loop_start = m->bookmarks[m->prev_idx].get_time();
			m->loop_end = m->bookmarks[m->next_idx].get_time();
			ret = true;
		}
	}

	return ret;
}

MetaData Bookmarks::current_track() const
{
	return m->md;
}

const QList<Bookmark>& Bookmarks::bookmarks() const
{
	return m->bookmarks;
}

int Bookmarks::get_size() const
{
	return m->bookmarks.size();
}
