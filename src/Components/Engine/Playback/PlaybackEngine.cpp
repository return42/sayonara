/* PlaybackEngine.cpp */

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

#include "PlaybackEngine.h"
#include "PlaybackPipeline.h"
#include "StreamRecorder.h"

#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"

#include <QUrl>


PlaybackEngine::PlaybackEngine(QObject* parent) :
	Engine(parent)
{
	_pipeline = nullptr;
	_other_pipeline = nullptr;

	_name = EngineName::PlaybackEngine;

	_playing_stream = false;

	_sr_active = false;
	_sr_wanna_record = false;
	_stream_recorder = new StreamRecorder(this);

	_gapless_state = GaplessState::NoGapless;

	_level_receiver = nullptr;
	_spectrum_receiver = nullptr;


	_timer = new QTimer();
	_timer->setTimerType(Qt::PreciseTimer);
	_timer->setSingleShot(true);

	connect(_timer, &QTimer::timeout, this, &PlaybackEngine::gapless_timed_out);

	REGISTER_LISTENER(Set::Engine_SR_Active, _streamrecorder_active_changed);
}


PlaybackEngine::~PlaybackEngine() {

	if(_stream_recorder->is_recording()){
		set_streamrecorder_recording(false);
	}

	if(_timer){
		delete _timer;
	}

	delete _pipeline;
	delete _stream_recorder;
}


bool PlaybackEngine::init() {

	gst_init(0, 0);

	_pipeline = new PlaybackPipeline(this);
	if(!_pipeline->init()){
		return false;
	}

	_other_pipeline = nullptr;

	connect(_pipeline, &PlaybackPipeline::sig_about_to_finish, this, &PlaybackEngine::set_about_to_finish);
	connect(_pipeline, &PlaybackPipeline::sig_pos_changed_ms, this, &PlaybackEngine::set_cur_position_ms);
	connect(_pipeline, &PlaybackPipeline::sig_data, this, &PlaybackEngine::sig_data);

	REGISTER_LISTENER(Set::Engine_Gapless, _gapless_changed);
	return true;
}


void PlaybackEngine::change_track_gapless(const MetaData& md) {

	set_uri(md.filepath());
	_md = md;

	qint64 time_to_go = _other_pipeline->get_time_to_go();

	if(time_to_go <= 0){
		_pipeline->play();
	}

	else{
		_timer->setInterval(time_to_go);
		_timer->start();

		sp_log(Log::Debug) << "Will start playing in " << time_to_go << "msec";
	}

	change_gapless_state(GaplessState::TrackFetched);
}

void PlaybackEngine::change_track(const QString& filepath) {

	bool got_md;
	MetaData md;

	md.set_filepath(filepath);
	got_md = Tagging::getMetaDataOfFile(md);

	if( !got_md ) {
		stop();
	}

	else{
		change_track(md);
	}
}


void PlaybackEngine::change_track(const MetaData& md) {

	bool success;
	emit sig_pos_changed_s(0);

	if(_timer){
		_timer->stop();
	}

	if( _gapless_state == GaplessState::AboutToFinish ) {
		change_track_gapless(md);
		return;
	}

	if(_other_pipeline){
		_other_pipeline->stop();
	}

	success = set_uri(md.filepath());
	if (!success){
		return;
	}

	_md = md;
	_cur_pos_ms = 0;

	change_gapless_state(GaplessState::Playing);
}


bool PlaybackEngine::set_uri(const QString& filepath) {

	bool success = false;
	QUrl url;

	if(_uri){
		g_free(_uri);
		_uri = nullptr;
	}

	_playing_stream = Helper::File::is_www(filepath);

	// stream, but don't wanna record
	// stream is already uri
	if (_playing_stream) {

		url = QUrl(filepath);
		_uri = g_strdup(url.toString().toUtf8().data());
	}

	// no stream (not quite right because of mms, rtsp or other streams
	// normal filepath -> no uri
	else if (!filepath.contains("://")) {

		url = QUrl::fromLocalFile(filepath);
		_uri = g_strdup(url.url().toUtf8().data());
	}

	else {
		_uri = g_strdup(filepath.toUtf8().data());
	}

	if(g_utf8_strlen(_uri, 1024) == 0){
		sp_log(Log::Warning) << "uri = 0";
		return false;
	}

	success = _pipeline->set_uri(_uri);

	return success;
}


void PlaybackEngine::play() {

	if( _gapless_state == GaplessState::AboutToFinish ||
		_gapless_state == GaplessState::TrackFetched )
	{
		return;
	}

	_pipeline->play();

	if(_sr_active && _stream_recorder->is_recording()){
		set_streamrecorder_recording(true);
	}

}


void PlaybackEngine::stop() {

	change_gapless_state(GaplessState::Playing);

	if(_timer){
		_timer->stop();
	}

	sp_log(Log::Info) << "Playback Engine: stop";
	_pipeline->stop();

	if(_other_pipeline){
		_other_pipeline->stop();
	}

	if(_sr_active && _stream_recorder->is_recording()){
		set_streamrecorder_recording(false);
	}


	emit sig_pos_changed_s(0);
}


void PlaybackEngine::pause() {
	_pipeline->pause();
}


void PlaybackEngine::jump_abs_ms(quint64 pos_ms) {
	_pipeline->seek_abs(pos_ms * GST_MSECOND);
}

void PlaybackEngine::jump_rel_ms(quint64 ms) {

	quint64 new_time_ms = _pipeline->get_position_ms() + ms;
	_pipeline->seek_abs(new_time_ms * GST_MSECOND);
}


void PlaybackEngine::jump_rel(double percent) {
	_pipeline->seek_rel(percent, _md.length_ms * GST_MSECOND);
}


void PlaybackEngine::change_equalizer(int band, int val) {

	double new_val;
	if (val > 0) {
		new_val = val * 0.25;
	}

	else{
		new_val = val * 0.75;
	}

	QString band_name = QString("band") + QString::number(band);
	_pipeline->set_eq_band(band_name, new_val);

	if(_other_pipeline){
		_other_pipeline->set_eq_band(band_name, new_val);
	}
}



void PlaybackEngine::buffering(int progress)
{
	if(Helper::File::is_www(_md.filepath())){
		emit sig_buffering(progress);
	}
}


void PlaybackEngine::set_cur_position_ms(qint64 pos_ms) {

	// the new track is requested so we won't display the position
	// of the old track, because probably the new track is already
	// displayed and active in playlist
	if( (_gapless_state == GaplessState::AboutToFinish) ||
		(_gapless_state == GaplessState::TrackFetched) )
	{
		emit sig_pos_changed_s(0);
		return;
	}

	if(pos_ms < 0 && Helper::File::is_www(_md.filepath())){
		return;
	}

	qint32 pos_sec;
	qint32 cur_pos_sec;

	pos_sec = pos_ms / 1000;
	cur_pos_sec = _cur_pos_ms / 1000;

	if ( cur_pos_sec == pos_sec ){
		return;
	}

	_cur_pos_ms = pos_ms;

	emit sig_pos_changed_s( pos_sec );
}


void PlaybackEngine::set_track_ready(){
	update_duration();

	emit sig_track_ready();
}




void PlaybackEngine::set_about_to_finish(qint64 time2go) {

	Q_UNUSED(time2go)


	if( _gapless_state == GaplessState::NoGapless ||
		_gapless_state == GaplessState::AboutToFinish )
	{
		return;
	}

	sp_log(Log::Debug) << "About to finish: " << (int) _gapless_state;
	change_gapless_state(GaplessState::AboutToFinish);

	// switch pipelines
	std::swap(_pipeline, _other_pipeline);

	emit sig_track_finished();
	emit sig_pos_changed_ms(0);
}


void PlaybackEngine::set_track_finished() {

	// fetch new track
	if( _gapless_state == GaplessState::NoGapless ||
		_gapless_state == GaplessState::Playing)
	{
		emit sig_track_finished();
	}

	// track already fetched, stop the pipeline
	// new pipeline will be activated automatically
	// by timer
	else if(_gapless_state == GaplessState::AboutToFinish ||
			_gapless_state == GaplessState::TrackFetched)
	{
		sp_log(Log::Debug) << "Old track finished";

		_other_pipeline->stop();
		_cur_pos_ms = 0;
		change_gapless_state(GaplessState::Playing);
	}

	emit sig_pos_changed_ms(0);
}

void PlaybackEngine::gapless_timed_out() {

	sp_log(Log::Debug) << "Timer timed out";
	_pipeline->play();
}


void PlaybackEngine::_gapless_changed() {

	bool gapless = _settings->get(Set::Engine_Gapless);

	if(gapless) {

		if(!_other_pipeline) {

			_other_pipeline = new PlaybackPipeline(this);
			if(!_other_pipeline->init()){
				change_gapless_state(GaplessState::NoGapless);
				return;
			}

			connect(_other_pipeline, &PlaybackPipeline::sig_about_to_finish, this, &PlaybackEngine::set_about_to_finish);
			connect(_other_pipeline, &PlaybackPipeline::sig_pos_changed_ms, this, &PlaybackEngine::set_cur_position_ms);
			connect(_other_pipeline, &PlaybackPipeline::sig_data, this, &PlaybackEngine::sig_data);
		}

		change_gapless_state(GaplessState::Playing);
	}

	else {
		change_gapless_state(GaplessState::NoGapless);
	}
}


void PlaybackEngine::change_gapless_state(GaplessState state)
{
	if( !_settings->get(Set::Engine_Gapless) ){
		_gapless_state = GaplessState::NoGapless;
	}

	else{
		_gapless_state = state;
	}
}




void PlaybackEngine::set_speed(float f) {
	_pipeline->set_speed(f);

	if(_other_pipeline){
		_other_pipeline->set_speed(f);
	}
}


void PlaybackEngine::_streamrecorder_active_changed(){

	_sr_active = _settings->get(Set::Engine_SR_Active);

	if(!_sr_active){
		set_streamrecorder_recording(false);
	}
}


void PlaybackEngine::set_streamrecorder_recording(bool b){

	QString dst_file;

	if(_stream_recorder->is_recording() != b){
		_stream_recorder->record(b);
	}

	if(b){
		dst_file = _stream_recorder->change_track(_md);
		if(dst_file.isEmpty()){
			return;
		}
	}
	else {
		dst_file = "";
	}

	if(!_pipeline) return;

	_pipeline->set_streamrecorder_path(dst_file);

}

void PlaybackEngine::set_n_sound_receiver(int num_sound_receiver)
{
	_pipeline->set_n_sound_receiver(num_sound_receiver);

	if(_other_pipeline){
		_other_pipeline->set_n_sound_receiver(num_sound_receiver);
	}
}


void PlaybackEngine::update_md(const MetaData& md){

	if(!Helper::File::is_www( _md.filepath() )) {
		return;
	}

	if(md.title.isEmpty()) {
		return;
	}

	if(md.title == _md.title) {
		return;
	}

	_cur_pos_ms = 0;
	_md.title = md.title;

	emit sig_md_changed(_md);

	if(_sr_active && _stream_recorder->is_recording()){
		set_streamrecorder_recording(true);
	}
}


void PlaybackEngine::update_duration() {

	_pipeline->refresh_duration();

	if( _gapless_state == GaplessState::AboutToFinish ||
		_gapless_state == GaplessState::TrackFetched )
	{
		return;
	}

	qint64 duration_ms = _pipeline->get_duration_ms();
	quint32 duration_s = duration_ms / 1000;
	quint32 md_duration_s = _md.length_ms / 1000;

	if(!between(duration_s, 1, 1500000)){
		return;
	}

	if(duration_s == md_duration_s) {
		return;
	}

	_md.length_ms = duration_ms;

	emit sig_dur_changed(_md);
}


void PlaybackEngine::update_bitrate(quint32 br){

	if( br / 1000 <= 0) {
		return;
	}

	if( br / 1000 == _md.bitrate / 1000) {
		return;
	}

	_md.bitrate = br;
	emit sig_br_changed(_md);
}


void PlaybackEngine::set_spectrum_receiver(SpectrumReceiver* receiver){
	_spectrum_receiver = receiver;
}

void PlaybackEngine::set_spectrum(const QVector<float>& vals){
	if(!_spectrum_receiver){
		return;
	}

	_spectrum_receiver->set_spectrum(vals);
}


void PlaybackEngine::set_level_receiver(LevelReceiver* receiver){
	_level_receiver = receiver;
}

void PlaybackEngine::set_level(float left, float right){
	if(!_level_receiver){
		return;
	}

	_level_receiver->set_level(left, right);
}
