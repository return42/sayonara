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
#include "Components/Engine/Callbacks/EngineCallbacks.h"

#include "Utils/MetaData/MetaData.h"
#include "Utils/FileUtils.h"
#include "Utils/Playlist/PlaylistMode.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"

#include <algorithm>

struct PlaybackEngine::Private
{
    PlaybackPipeline*			pipeline=nullptr;
    PlaybackPipeline*			other_pipeline=nullptr;

    QList<LevelReceiver*>		level_receiver;
    QList<SpectrumReceiver*>	spectrum_receiver;

    QTimer*						gapless_timer=nullptr;
    GaplessState				gapless_state;

    StreamRecorder*				stream_recorder=nullptr;
    bool						sr_active;

    Private(Engine* parent) :
        gapless_state(GaplessState::Stopped),
        sr_active(false)
    {
        stream_recorder = new StreamRecorder(parent);

        gapless_timer = new QTimer();
        gapless_timer->setTimerType(Qt::PreciseTimer);
        gapless_timer->setSingleShot(true);
    }

    ~Private()
    {
        while(gapless_timer && gapless_timer->isActive())
        {
            gapless_timer->stop();
        }

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
}


PlaybackEngine::~PlaybackEngine()
{
    if(m->stream_recorder->is_recording())
    {
        set_streamrecorder_recording(false);
    }
}


bool PlaybackEngine::init()
{
    gst_init(0, 0);

    bool success = init_pipeline(&m->pipeline);
    if(!success){
        return false;
    }

    Set::listen(Set::Engine_SR_Active, this, &PlaybackEngine::s_streamrecorder_active_changed);
    Set::listen(Set::PL_Mode, this, &PlaybackEngine::s_gapless_changed);
    Set::listen(Set::Engine_CrossFaderActive, this, &PlaybackEngine::s_gapless_changed);

    connect(m->gapless_timer, &QTimer::timeout, [=](){
        m->pipeline->play();
    });

    return true;
}

bool PlaybackEngine::init_pipeline(PlaybackPipeline** pipeline)
{
    if(*pipeline){
        return true;
    }

    *pipeline = new PlaybackPipeline(this);
    PlaybackPipeline* p = *pipeline;

    if(!p->init()){
        change_gapless_state(GaplessState::NoGapless);
        return false;
    }

    connect(p, &PlaybackPipeline::sig_about_to_finish, this, &PlaybackEngine::set_track_almost_finished);
    connect(p, &PlaybackPipeline::sig_pos_changed_ms, this, &PlaybackEngine::cur_pos_ms_changed);
    connect(p, &PlaybackPipeline::sig_data, this, &PlaybackEngine::sig_data);

    return true;
}

bool PlaybackEngine::change_track_crossfading(const MetaData& md)
{
    std::swap(m->pipeline, m->other_pipeline);

    m->other_pipeline->fade_out();

    if (!Engine::change_track(md)) {
        return false;
    }

    m->pipeline->fade_in();
    change_gapless_state(GaplessState::Playing);

    return true;
}

bool PlaybackEngine::change_track_gapless(const MetaData& md)
{
    std::swap(m->pipeline, m->other_pipeline);

    if (!Engine::change_track(md)) {
        return false;
    }

    int64_t time_to_go = m->other_pipeline->get_time_to_go();
    if(time_to_go <= 0) {
        m->pipeline->play();
    }

    else {
        m->gapless_timer->setInterval(time_to_go);
        m->gapless_timer->start();

        sp_log(Log::Develop, this) << "Will start playing in " << time_to_go << "msec";
    }

    change_gapless_state(GaplessState::TrackFetched);

    return true;
}

bool PlaybackEngine::change_track_immediatly(const MetaData& md)
{
    if(m->other_pipeline) {
        m->other_pipeline->stop();
    }

    return Engine::change_track(md);
}

bool PlaybackEngine::change_track(const MetaData& md)
{
    if(m->gapless_timer){
        m->gapless_timer->stop();
    }

    bool crossfader_active = _settings->get(Set::Engine_CrossFaderActive);
    if(m->gapless_state != GaplessState::Stopped && crossfader_active)
    {
        return change_track_crossfading(md);
    }

    else if(m->gapless_state == GaplessState::AboutToFinish )
    {
        return change_track_gapless(md);
    }

    return change_track_immediatly(md);
}

bool PlaybackEngine::change_metadata(const MetaData& md)
{
    bool success = Engine::change_metadata(md);
    if(!success)
    {
        change_gapless_state(GaplessState::Stopped);
    }

    return success;
}

bool PlaybackEngine::change_uri(char* uri)
{
    return m->pipeline->set_uri(uri);
}

void PlaybackEngine::play()
{
    if( m->gapless_state == GaplessState::AboutToFinish ||
        m->gapless_state == GaplessState::TrackFetched)
    {
        return;
    }

    m->pipeline->play();

    if(m->sr_active && m->stream_recorder->is_recording()){
        set_streamrecorder_recording(true);
    }

    change_gapless_state(GaplessState::Playing);
}


void PlaybackEngine::stop()
{
    change_gapless_state(GaplessState::Stopped);

    if(m->gapless_timer){
        m->gapless_timer->stop();
    }

    sp_log(Log::Info, this) << "Playback Engine: stop";
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

    Engine::stop();
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
    m->pipeline->seek_rel(percent, metadata().length_ms * GST_MSECOND);
}



void PlaybackEngine::cur_pos_ms_changed(int64_t pos_ms)
{
    if(sender() != m->pipeline){
        return;
    }

    if(pos_ms < 0 && Util::File::is_www(metadata().filepath())){
        return;
    }

    Engine::set_current_position_ms(pos_ms);
}


void PlaybackEngine::set_track_ready(GstElement* src)
{
    if(m->pipeline->has_element(src)){
        Engine::set_track_ready(src);
    }
}

void PlaybackEngine::set_track_almost_finished(int64_t time2go)
{
    Q_UNUSED(time2go)

    if(sender() != m->pipeline){
        return;
    }

    if( m->gapless_state == GaplessState::NoGapless ||
        m->gapless_state == GaplessState::AboutToFinish )
    {
        Engine::set_track_almost_finished(time2go);
        return;
    }

    sp_log(Log::Debug, this) << "About to finish: " <<
                                (int) m->gapless_state << " (" << time2go << "ms)";

    change_gapless_state(GaplessState::AboutToFinish);

    bool crossfade = _settings->get(Set::Engine_CrossFaderActive);
    if(crossfade){
        m->pipeline->fade_out();
    }

    emit sig_track_finished();
}


void PlaybackEngine::set_track_finished(GstElement* src)
{
    if(m->pipeline->has_element(src))
    {
        Engine::set_track_finished(src);
    }

    if(m->other_pipeline && m->other_pipeline->has_element(src))
    {
        sp_log(Log::Debug, this) << "Old track finished";

        m->other_pipeline->stop();

        change_gapless_state(GaplessState::Playing);
    }
}


void PlaybackEngine::set_equalizer(int band, int val)
{
    m->pipeline->set_eq_band(band, val);

    if(m->other_pipeline){
        m->other_pipeline->set_eq_band(band, val);
    }
}


void PlaybackEngine::set_buffer_state(int progress, GstElement* src)
{
    if(!Util::File::is_www(metadata().filepath())){
        progress = -1;
    }

    if(!m->pipeline->has_element(src)){
        progress = -1;
    }

    Engine::set_buffer_state(progress, src);
}


void PlaybackEngine::s_gapless_changed()
{
    Playlist::Mode plm = _settings->get(Set::PL_Mode);
    bool gapless =	(Playlist::Mode::isActiveAndEnabled(plm.gapless()) ||
                     _settings->get(Set::Engine_CrossFaderActive));

    if(gapless)
    {
        bool success = init_pipeline(&m->other_pipeline);

        if(success){
            change_gapless_state(GaplessState::Stopped);
            return;
        }
    }

    change_gapless_state(GaplessState::NoGapless);
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


void PlaybackEngine::s_streamrecorder_active_changed()
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
        dst_file = m->stream_recorder->change_track(metadata());
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
        Engine::update_cover(img, src);
    }
}


void PlaybackEngine::update_metadata(const MetaData& md, GstElement* src)
{
    if(!m->pipeline->has_element(src)){
        return;
    }

    if(!Util::File::is_www( metadata().filepath() )) {
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

    if(metadata().title.compare(title) == 0)
    {
        return;
    }

    set_current_position_ms(0);

    MetaData md_update = metadata();
    if(splitted.size() == 2){
        md_update.set_artist(splitted[0].trimmed());
        md_update.title = splitted[1].trimmed();
    }

    else {
        md_update.title = md.title;
    }

    Engine::update_metadata(md_update, src);

    if(m->sr_active && m->stream_recorder->is_recording()){
        set_streamrecorder_recording(true);
    }
}


void PlaybackEngine::update_duration(int64_t duration_ms, GstElement* src)
{
    if(! m->pipeline->has_element(src)){
        return;
    }

    m->pipeline->update_duration_ms(duration_ms, src);

    Engine::update_duration(duration_ms, src);

}


void PlaybackEngine::update_bitrate(uint32_t br, GstElement* src)
{
    if(!m->pipeline->has_element(src)){
        return;
    }

    Engine::update_bitrate(br, src);
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

