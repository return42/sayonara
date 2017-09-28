/* PlaybackEngine.cpp */

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

#include "PlaybackEngine.h"
#include "PlaybackPipeline.h"
#include "StreamRecorder.h"
#include "SoundOutReceiver.h"

#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"
#include "Helper/Playlist/PlaylistMode.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

#include <QUrl>
#include <algorithm>

struct PlaybackEngine::Private
{
    PlaybackPipeline*			pipeline=nullptr;
    PlaybackPipeline*			other_pipeline=nullptr;

    QList<LevelReceiver*>		level_receiver;
    QList<SpectrumReceiver*>	spectrum_receiver;

    QTimer*						gapless_timer=nullptr;
    GaplessState				gapless_state;

    bool						sr_active;

    StreamRecorder*				stream_recorder=nullptr;

    Private(Engine* parent) :
        gapless_state(GaplessState::NoGapless),
        sr_active(false)
    {
       stream_recorder = new StreamRecorder(parent);

       gapless_timer = new QTimer();
       gapless_timer->setTimerType(Qt::PreciseTimer);
       gapless_timer->setSingleShot(true);
    }

    ~Private()
    {
        if(gapless_timer){
            delete gapless_timer; gapless_timer = nullptr;
        }

        delete pipeline; pipeline = nullptr;
        if(other_pipeline)
        {
            delete other_pipeline; other_pipeline = nullptr;
        }

        delete stream_recorder; stream_recorder = nullptr;
    }
};

PlaybackEngine::PlaybackEngine(QObject* parent) :
    Engine(EngineName::PlaybackEngine, parent)
{
    m = Pimpl::make<Private>(this);

    connect(m->gapless_timer, &QTimer::timeout, this, &PlaybackEngine::gapless_timed_out);
}


PlaybackEngine::~PlaybackEngine()
{
    if(m->stream_recorder->is_recording()){
        set_streamrecorder_recording(false);
    }
}


bool PlaybackEngine::init()
{
	gst_init(0, 0);

    m->pipeline = new PlaybackPipeline(this);
    if(!m->pipeline->init()){
		return false;
	}

    m->other_pipeline = nullptr;

    connect(m->pipeline, &PlaybackPipeline::sig_about_to_finish, this, &PlaybackEngine::set_about_to_finish);
    connect(m->pipeline, &PlaybackPipeline::sig_pos_changed_ms, this, &PlaybackEngine::set_cur_position_ms);
    connect(m->pipeline, &PlaybackPipeline::sig_data, this, &PlaybackEngine::sig_data);

	REGISTER_LISTENER(Set::PL_Mode, _playlist_mode_changed);
	REGISTER_LISTENER(Set::Engine_SR_Active, _streamrecorder_active_changed);
	REGISTER_LISTENER(Set::Engine_CrossFaderActive, _playlist_mode_changed);

	return true;
}

void PlaybackEngine::init_other_pipeline()
{
    if(!m->other_pipeline)
	{
        m->other_pipeline = new PlaybackPipeline(this);

        if(!m->other_pipeline->init()){
			change_gapless_state(GaplessState::NoGapless);
			return;
		}

        connect(m->other_pipeline, &PlaybackPipeline::sig_about_to_finish, this, &PlaybackEngine::set_about_to_finish);
        connect(m->other_pipeline, &PlaybackPipeline::sig_pos_changed_ms, this, &PlaybackEngine::set_cur_position_ms);
        connect(m->other_pipeline, &PlaybackPipeline::sig_data, this, &PlaybackEngine::sig_data);
	}
}


void PlaybackEngine::change_track_gapless(const MetaData& md)
{
	set_uri(md.filepath());
	_md = md;

	bool crossfade = _settings->get(Set::Engine_CrossFaderActive);

	if(crossfade){
        m->pipeline->play();
        m->pipeline->fade_in();
	}

	else {
        int64_t time_to_go = m->other_pipeline->get_time_to_go();

		 if(time_to_go <= 0){
            m->pipeline->play();
		 }

		 else{
             m->gapless_timer->setInterval(time_to_go);
             m->gapless_timer->start();

			 sp_log(Log::Debug, this) << "Will start playing in " << time_to_go << "msec";
		 }
	}

	change_gapless_state(GaplessState::TrackFetched);
}

void PlaybackEngine::change_track(const QString& filepath)
{
	MetaData md(filepath);

	bool got_md = Tagging::getMetaDataOfFile(md);
	if( !got_md ) {
		stop();
	}

	else{
		change_track(md);
	}
}


void PlaybackEngine::change_track(const MetaData& md)
{
	bool success;
	emit sig_pos_changed_s(0);

    if(m->gapless_timer){
        m->gapless_timer->stop();
	}

    if( m->gapless_state == GaplessState::AboutToFinish ) {
		change_track_gapless(md);
		return;
	}

    if(m->other_pipeline){
        m->other_pipeline->stop();
	}

	success = set_uri(md.filepath());
	if (!success){
		return;
	}

    _md = md;
    _cur_pos_ms = 0;

	change_gapless_state(GaplessState::Playing);
}


bool PlaybackEngine::set_uri(const QString& filepath)
{
    bool success = Engine::set_uri(filepath);
    if(!success){
        return false;
    }

    return m->pipeline->set_uri(get_uri());
}


void PlaybackEngine::play()
{
    if( m->gapless_state == GaplessState::AboutToFinish ||
        m->gapless_state == GaplessState::TrackFetched )
	{
		return;
	}

    m->pipeline->play();

    if(m->sr_active && m->stream_recorder->is_recording()){
		set_streamrecorder_recording(true);
	}
}


void PlaybackEngine::stop()
{
	change_gapless_state(GaplessState::Playing);

    if(m->gapless_timer){
        m->gapless_timer->stop();
	}

	sp_log(Log::Info) << "Playback Engine: stop";
    m->pipeline->stop();

    if(m->other_pipeline){
        m->other_pipeline->stop();
	}

	if(m->gapless_timer){
		m->gapless_timer->stop();
	}

    if(m->sr_active && m->stream_recorder->is_recording()){
		set_streamrecorder_recording(false);
	}

	emit sig_buffering(-1);
	emit sig_pos_changed_s(0);
}


void PlaybackEngine::pause()
{
    m->pipeline->pause();
}


void PlaybackEngine::jump_abs_ms(uint64_t pos_ms)
{
    m->pipeline->seek_abs(pos_ms * GST_MSECOND);
}

void PlaybackEngine::jump_rel_ms(uint64_t ms)
{
    uint64_t new_time_ms = m->pipeline->get_source_position_ms() + ms;
    m->pipeline->seek_abs(new_time_ms * GST_MSECOND);
}


void PlaybackEngine::jump_rel(double percent)
{
    m->pipeline->seek_rel(percent, _md.length_ms * GST_MSECOND);
}


void PlaybackEngine::set_equalizer(int band, int val)
{
	double new_val;
	if (val > 0) {
		new_val = val * 0.25;
	}

	else{
		new_val = val * 0.75;
	}

	QString band_name = QString("band") + QString::number(band);
    m->pipeline->set_eq_band(band_name, new_val);

    if(m->other_pipeline){
        m->other_pipeline->set_eq_band(band_name, new_val);
	}
}


void PlaybackEngine::set_buffer_state(int progress, GstElement* src)
{
	if(!Helper::File::is_www(_md.filepath())){
		emit sig_buffering(-1);
		return;
	}

    if(!m->pipeline->has_element(src)){
		emit sig_buffering(-1);
		return;
	}

	emit sig_buffering(progress);
}


void PlaybackEngine::set_cur_position_ms(int64_t pos_ms)
{
    if(sender() != m->pipeline){
		return;
	}

	if(pos_ms < 0 && Helper::File::is_www(_md.filepath())){
		return;
	}

	int32_t pos_sec = pos_ms / 1000;
	int32_t cur_pos_sec = _cur_pos_ms / 1000;

	if ( cur_pos_sec == pos_sec ){
		return;
	}

	_cur_pos_ms = pos_ms;

	emit sig_pos_changed_s( pos_sec );
}


void PlaybackEngine::set_track_ready(GstElement* src)
{
	update_duration(src);

    if(m->pipeline->has_element(src)){
		emit sig_track_ready();
	}
}


void PlaybackEngine::set_about_to_finish(int64_t time2go)
{
	Q_UNUSED(time2go)

    if(sender() != m->pipeline){
		return;
	}

    if( m->gapless_state == GaplessState::NoGapless ||
        m->gapless_state == GaplessState::AboutToFinish )
	{
		return;
	}

    sp_log(Log::Debug, this) << "About to finish: " << (int) m->gapless_state << " (" << time2go << "ms)";
	change_gapless_state(GaplessState::AboutToFinish);

	bool crossfade = _settings->get(Set::Engine_CrossFaderActive);
	if(crossfade){
        m->pipeline->fade_out();
	}

	// switch pipelines
    if(!m->other_pipeline){
        init_other_pipeline();
	}

    std::swap(m->pipeline, m->other_pipeline);

	emit sig_track_finished();
	emit sig_pos_changed_ms(0);
}


void PlaybackEngine::set_track_finished(GstElement* src)
{
    if(m->pipeline->has_element(src))
    {
		emit sig_track_finished();
		emit sig_pos_changed_ms(0);
	}

    if(m->other_pipeline && m->other_pipeline->has_element(src))
    {
		sp_log(Log::Debug, this) << "Old track finished";

        m->other_pipeline->stop();
		_cur_pos_ms = 0;
		change_gapless_state(GaplessState::Playing);
	}
}

void PlaybackEngine::gapless_timed_out()
{
    m->pipeline->play();
}


void PlaybackEngine::_playlist_mode_changed()
{
	Playlist::Mode plm = _settings->get(Set::PL_Mode);
	bool gapless =	Playlist::Mode::isActiveAndEnabled(plm.gapless()) ||
					_settings->get(Set::Engine_CrossFaderActive);

	if(gapless) {
        init_other_pipeline();
		change_gapless_state(GaplessState::Playing);
	}

	else {
		change_gapless_state(GaplessState::NoGapless);
	}
}


void PlaybackEngine::change_gapless_state(GaplessState state)
{
	Playlist::Mode plm = _settings->get(Set::PL_Mode);

	bool gapless = Playlist::Mode::isActiveAndEnabled(plm.gapless());
	bool crossfader = _settings->get(Set::Engine_CrossFaderActive);

    m->gapless_state = state;

	if(!gapless && !crossfader) {
        m->gapless_state = GaplessState::NoGapless;
	}
}


void PlaybackEngine::_streamrecorder_active_changed()
{
    m->sr_active = _settings->get(Set::Engine_SR_Active);

    if(!m->sr_active){
		set_streamrecorder_recording(false);
	}
}


void PlaybackEngine::set_streamrecorder_recording(bool b)
{
	QString dst_file;

    if(m->stream_recorder->is_recording() != b){
        m->stream_recorder->record(b);
	}

    if(b)
    {
        dst_file = m->stream_recorder->change_track(_md);
		if(dst_file.isEmpty()){
			return;
		}
	}

    if(m->pipeline) {
        m->pipeline->set_streamrecorder_path(dst_file);
	}
}

void PlaybackEngine::set_n_sound_receiver(int num_sound_receiver)
{
    m->pipeline->set_n_sound_receiver(num_sound_receiver);

    if(m->other_pipeline)
    {
        m->other_pipeline->set_n_sound_receiver(num_sound_receiver);
	}
}

void PlaybackEngine::update_cover(const QImage& img, GstElement* src)
{
    if( m->pipeline->has_element(src) )
	{
		emit sig_cover_changed(img);
	}
}


void PlaybackEngine::update_md(const MetaData& md, GstElement* src)
{
    if(!m->pipeline->has_element(src)){
		return;
	}

	if(!Helper::File::is_www( _md.filepath() )) {
		return;
	}

	if(md.title.isEmpty()) {
		return;
	}

	QString title = md.title;
	QStringList splitted = md.title.split("-");
	if(splitted.size() == 2) {
		title = splitted[1].trimmed();
	}

	if(	title == _md.title)
	{
		return;
	}

	_cur_pos_ms = 0;
	if(splitted.size() == 2){
		_md.artist = splitted[0].trimmed();
		_md.title = splitted[1].trimmed();
	}

	else {
		_md.title = md.title;
	}

	emit sig_md_changed(_md);

    if(m->sr_active && m->stream_recorder->is_recording()){
		set_streamrecorder_recording(true);
	}
}


void PlaybackEngine::update_duration(GstElement* src)
{
    if(! m->pipeline->has_element(src)){
		return;
	}

    m->pipeline->refresh_duration();

    int64_t duration_ms = m->pipeline->get_duration_ms();
	uint32_t duration_s = (duration_ms >> 10);
	uint32_t md_duration_s = (_md.length_ms >> 10);

	if(duration_s == 0 || duration_s > 1500000){
		return;
	}

	if(duration_s == md_duration_s) {
		return;
	}

	_md.length_ms = duration_ms;

	emit sig_dur_changed(_md);
}


void PlaybackEngine::update_bitrate(uint32_t br, GstElement* src)
{
    if(!m->pipeline->has_element(src)){
		return;
	}

	if( br / 1000 <= 0) {
		return;
	}

	if( br / 1000 == _md.bitrate / 1000) {
		return;
	}

	_md.bitrate = br;
	emit sig_br_changed(_md);
}


void PlaybackEngine::add_spectrum_receiver(SpectrumReceiver* receiver)
{
    m->spectrum_receiver << receiver;
}

int PlaybackEngine::get_spectrum_bins() const
{
	return _settings->get(Set::Engine_SpectrumBins);
}

void PlaybackEngine::set_spectrum(const QList<float>& vals)
{
    for(SpectrumReceiver* rcv : m->spectrum_receiver)
    {
		if(rcv){
			rcv->set_spectrum(vals);
		}
	}
}


void PlaybackEngine::add_level_receiver(LevelReceiver* receiver)
{
    m->level_receiver << receiver;
}

void PlaybackEngine::set_level(float left, float right)
{
    for(LevelReceiver* rcv : m->level_receiver)
    {
		if(rcv){
			rcv->set_level(left, right);
		}
	}
}

