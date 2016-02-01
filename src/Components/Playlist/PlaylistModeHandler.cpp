/* PlaylistModeHandler.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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

#include "playlist/PlaylistModeHandler.h"
#include "Helper/CSettingsStorage.h"

#define PMH_CHANGE_AND_TEST(x) PlaylistMode tmp_mode_ = _mode; \
								x ; \
								if( tmp_mode_ == _mode ) return


PlaylistModeHandler::PlaylistModeHandler(QObject* parent) : 
	QObject(parent) {

	_mode = CSettingsStorage::getInstance()->getPlaylistMode();

}

PlaylistModeHandler::~PlaylistModeHandler(){
	CSettingsStorage::getInstance()->setPlaylistMode( _mode );
}


void PlaylistModeHandler::save_and_emit(){

	CSettingsStorage::getInstance()->setPlaylistMode( _mode );
	emit sig_playlist_mode_changed( _mode );
}

void PlaylistModeHandler::set_mode(const PlaylistMode& mode){

	PMH_CHANGE_AND_TEST (_mode = mode);
	save_and_emit();
}

void PlaylistModeHandler::enable_repeat(){
	

	PMH_CHANGE_AND_TEST (_mode.repAll = true);
	save_and_emit();
}

void PlaylistModeHandler::disable_repeat(){
	PMH_CHANGE_AND_TEST (_mode.repAll = false);
	save_and_emit();
}

void PlaylistModeHandler::toggle_repeat( bool b ){
	PMH_CHANGE_AND_TEST (_mode.repAll = b);
	save_and_emit();
}

void PlaylistModeHandler::enable_shuffle(){
	PMH_CHANGE_AND_TEST (_mode.shuffle = true);
	save_and_emit();
}

void PlaylistModeHandler::disable_shuffle(){
	PMH_CHANGE_AND_TEST (_mode.shuffle = false);
	save_and_emit();
}

void PlaylistModeHandler::toggle_shuffle( bool b ){
	PMH_CHANGE_AND_TEST (_mode.shuffle = b);
	save_and_emit();
}

void PlaylistModeHandler::enable_append(){
	PMH_CHANGE_AND_TEST (_mode.append = true);
	save_and_emit();
}

void PlaylistModeHandler::disable_append(){
	PMH_CHANGE_AND_TEST (_mode.append = false);
	save_and_emit();
}

void PlaylistModeHandler::toggle_append( bool b ){
	PMH_CHANGE_AND_TEST (_mode.append = b);
	save_and_emit();
}

void PlaylistModeHandler::enable_dynamic(){
	PMH_CHANGE_AND_TEST (_mode.dynamic = true);
	save_and_emit();
}

void PlaylistModeHandler::disable_dynamic(){
	PMH_CHANGE_AND_TEST (_mode.dynamic = false);
	save_and_emit();
}

void PlaylistModeHandler::toggle_dynamic( bool b ){
	PMH_CHANGE_AND_TEST (_mode.dynamic = b);
	save_and_emit();
}
void PlaylistModeHandler::enable_gapless(){
	PMH_CHANGE_AND_TEST (_mode.gapless = true);
	save_and_emit();
}

void PlaylistModeHandler::disable_gapless(){
	PMH_CHANGE_AND_TEST (_mode.gapless = false);
	save_and_emit();
}

void PlaylistModeHandler::toggle_gapless( bool b ){
	PMH_CHANGE_AND_TEST (_mode.gapless = b);
	save_and_emit();
}

