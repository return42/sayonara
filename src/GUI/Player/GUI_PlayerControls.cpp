// GUI_PlayerButtons.cpp

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

#include "GUI_Player.h"
#include "GUI_TrayIcon.h"
#include "GUI/Utils/IconLoader/IconLoader.h"
#include "Components/PlayManager/PlayManager.h"
#include "Utils/Utils.h"
#include "Utils/Settings/Settings.h"
#include "Utils/MetaData/MetaData.h"

#include <QToolTip>

#include <algorithm>

/** PLAYER BUTTONS **/
void GUI_Player::playstate_changed(PlayState state)
{
    switch(state)
    {
		case PlayState::Playing:
			played();
			break;
		case PlayState::Paused:
			paused();
			break;
		case PlayState::Stopped:
			stopped();
			break;
		default:
			break;
	}

	check_record_button_visible();
	return;
}


void GUI_Player::play_clicked()
{
    PlayManager::instance()->play_pause();
}


void GUI_Player::played()
{
    btn_play->setIcon(IconLoader::icon("media-playback-pause", "pause"));
}


void GUI_Player::paused()
{
    btn_play->setIcon(IconLoader::icon("media-playback-start", "play"));
}


void GUI_Player::stop_clicked()
{
    PlayManager::instance()->stop();
}


void GUI_Player::stopped()
{
	setWindowTitle("Sayonara");

    btn_play->setIcon(IconLoader::icon("media-playback-start", "play"));

	progress_widget->setCurrentIndex(0);

	lab_title->hide();
	lab_sayonara->show();

	lab_artist->hide();
	lab_writtenby->show();

	lab_album->hide();
	lab_version->show();

	lab_rating->hide();
	lab_copyright->show();

	sli_progress->setValue(0);
	sli_progress->setEnabled(false);

	lab_cur_time->setText("00:00");
	lab_max_time->clear();

	set_standard_cover();
}


void GUI_Player::prev_clicked()
{
    PlayManager::instance()->previous();
}


void GUI_Player::next_clicked()
{
    PlayManager::instance()->next();
}


void GUI_Player::rec_clicked(bool b)
{
    PlayManager::instance()->record(b);
}

void GUI_Player::rec_changed(bool b)
{
	btn_rec->setChecked(b);
}


void GUI_Player::buffering(int progress)
{
	sli_buffer->set_position(Gui::ProgressBar::Position::Middle);

	if(progress > 0 && progress < 100)
	{
		progress_widget->setCurrentIndex(1);

		sli_buffer->setMinimum(0);
		sli_buffer->setMaximum(100);
		sli_buffer->setValue(progress);

		lab_cur_time->setText(QString("%1%").arg(progress));
		lab_max_time->setVisible(false);
	}

	else if(progress == 0)
	{
		progress_widget->setCurrentIndex(1);

		sli_buffer->setMinimum(0);
		sli_buffer->setMaximum(0);
		sli_buffer->setValue(progress);

		lab_cur_time->setText("0%");
		lab_max_time->setVisible(false);
	}


	else
	{
		progress_widget->setCurrentIndex(0);

		sli_buffer->setMinimum(0);
		sli_buffer->setMaximum(0);

		lab_cur_time->clear();
		lab_max_time->setVisible(true);
	}
}


void GUI_Player::set_progress_tooltip(int val)
{
    uint64_t duration = PlayManager::instance()->duration_ms();
	int max = sli_progress->maximum();

	val = std::max(val, 0);
	val = std::min(max, val);

	double percent = (val * 1.0) / max;
	uint64_t cur_pos_ms =  (uint64_t) (percent * duration);
	QString cur_pos_string = Util::cvt_ms_to_string(cur_pos_ms);

	QToolTip::showText( QCursor::pos(), cur_pos_string );
}


void GUI_Player::set_cur_pos_label(int val)
{
    uint64_t duration = PlayManager::instance()->duration_ms();
	int max = sli_progress->maximum();

	val = std::max(val, 0);
	val = std::min(max, val);

	double percent = (val * 1.0) / max;
	uint64_t cur_pos_ms =  (uint64_t) (percent * duration);
	QString cur_pos_string = Util::cvt_ms_to_string(cur_pos_ms);

	lab_cur_time->setText(cur_pos_string);
}


void GUI_Player::set_total_time_label(int64_t total_time)
{
	QString length_str;
	if(total_time > 0){
		length_str = Util::cvt_ms_to_string(total_time, true);
	}

	lab_max_time->setText(length_str);
	sli_progress->setEnabled(total_time > 0);
}


void GUI_Player::file_info_changed()
{
    const MetaData& md = PlayManager::instance()->current_track();
	QString rating_text;

	if(md.bitrate / 1000 > 0){
		rating_text = QString::number(md.bitrate / 1000) + " kBit/s";
	}

	if(md.filesize > 0){
		if(!rating_text.isEmpty()){
			rating_text += ", ";
		}

		rating_text += QString::number( (double) (md.filesize / 1024) / 1024.0, 'f', 2) + " MB";
	}

	if( (_settings->get(Set::Engine_Pitch) != 440) &&
		_settings->get(Set::Engine_SpeedActive))
	{
		if(!rating_text.isEmpty()){
			rating_text += ", ";
		}

		rating_text += QString::number(_settings->get(Set::Engine_Pitch)) + "Hz";
	}

	lab_rating->setText(rating_text);
	lab_rating->setToolTip(rating_text);
}

void GUI_Player::seek(int val)
{
	val = std::max(val, 0);

	set_cur_pos_label(val);

	double percent = (val * 1.0) / sli_progress->maximum();
    PlayManager::instance()->seek_rel(percent);
}


void GUI_Player::cur_pos_changed(uint64_t pos_ms)
{
    uint64_t duration = PlayManager::instance()->duration_ms();
	int max = sli_progress->maximum();
	int new_val;

	if ( duration > 0 ) {
		new_val = ( pos_ms * max ) / (duration);
	}

	else if(pos_ms > duration) {
		new_val = 0;
	}

	else{
		return;
	}

	if(!sli_progress->is_busy()){
		QString cur_pos_string = Util::cvt_ms_to_string(pos_ms);
		lab_cur_time->setText(cur_pos_string);
		sli_progress->setValue(new_val);
	}
}


void GUI_Player::volume_slider_moved(int val)
{
    PlayManager::instance()->set_volume(val);
}


void GUI_Player::volume_changed(int val)
{
	setup_volume_button(val);
	sli_volume->setValue(val);
}


void GUI_Player::change_volume_by_tick(int val)
{
	if(val > 0){
		increase_volume();
	}
	else{
		decrease_volume();
	}
}


void GUI_Player::increase_volume()
{
    PlayManager::instance()->volume_up();
}


void GUI_Player::decrease_volume()
{
    PlayManager::instance()->volume_down();
}


void GUI_Player::setup_volume_button(int percent)
{
	QString but_name = "vol_";
	QString but_std_name = "vol_";

	if (percent <= 1) {
		but_name += QString("mute_dark");
		but_std_name = QString("audio-volume-muted");
	}

	else if (percent < 40) {
		but_name += QString("1_dark");
		but_std_name = QString("audio-volume-low");
	}

	else if (percent < 80) {
		but_name += QString("2_dark");
		but_std_name = QString("audio-volume-medium");
	}

	else {
		but_name += QString("3_dark");
		but_std_name = QString("audio-volume-high");
	}

    btn_mute->setIcon( IconLoader::icon(but_std_name, but_name));
}


void GUI_Player::mute_button_clicked()
{
	bool muted = _settings->get(Set::Engine_Mute);
    PlayManager::instance()->set_muted(!muted);
}


void GUI_Player::mute_changed(bool muted)
{
	int val;
	sli_volume->setDisabled(muted);

	if(muted){
		val = 0;
	}

	else {
		val = _settings->get(Set::Engine_Vol);
	}

	sli_volume->setValue(val);
	setup_volume_button(val);
}
