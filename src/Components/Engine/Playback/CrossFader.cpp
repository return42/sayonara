#include "CrossFader.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Helper.h"
#include <glib.h>
#include <functional>

struct FaderThreadData
{
    int		cycles;
    quint64	cycle_time_ms;
    std::function<void ()> fn;

    FaderThreadData(){
	cycles = 500;
	cycle_time_ms = 10;
    }

};


static void timer_thread(FaderThreadData* data){

    while(data->cycles > 0){

	Helper::sleep_ms(data->cycle_time_ms);

	data->cycles--;
	data->fn();
    }
}

CrossFader::CrossFader()
{
    _fading_time = 10000;
    _fade_overlap = 5000;
    _fade_mode = CrossFader::FadeMode::NoFading;
    _fader_data = new FaderThreadData();
}


void CrossFader::init_fader(){
    if(_fade_mode == CrossFader::FadeMode::NoFading){
	return;
    }


    if(_fader && _fader_data->cycles > 0){
	_fader_data->cycles = 0;
	_fader->join();

	delete _fader;
    }

    _fader_data->cycles = 500;
    _fader_data->cycle_time_ms = 10;
    _fader_data->fn = std::bind(&CrossFader::fader_timed_out, this);
    _fader = new std::thread(timer_thread, _fader_data);
}


void CrossFader::fade_in(){

    double volume = Settings::getInstance()->get(Set::Engine_Vol) / 100.0;

    _fade_mode = CrossFader::FadeMode::FadeIn;

    _fade_step = volume / 500.0;

    set_current_volume(0.0001);

    init_fader();
}

void CrossFader::fade_out(){
    double volume = Settings::getInstance()->get(Set::Engine_Vol) / 100.0;

    _fade_mode = CrossFader::FadeMode::FadeOut;
    _fade_step = volume / 500.0;

    set_current_volume( volume );

    init_fader();
}


void CrossFader::fader_timed_out(){
    if(_fade_mode == CrossFader::FadeMode::FadeIn){
	increase_volume();
    }

    else if(_fade_mode == CrossFader::FadeMode::FadeOut){
	decrease_volume();
    }
}



void CrossFader::increase_volume()
{
    double max_volume = Settings::getInstance()->get(Set::Engine_Vol) / 100.0;
    double volume = get_current_volume();

    volume += _fade_step;
    sp_log(Log::Debug) << "Folume: " << volume;

    if(volume > max_volume){
	abort_fader();

	return;
    }

    set_current_volume(volume);
}


void CrossFader::decrease_volume()
{
    double volume = get_current_volume();

    volume -= _fade_step;
    if(volume < 0.00001){

	abort_fader();
	return;
    }

    set_current_volume(volume);
}



quint64 CrossFader::get_fade_overlap() const
{
    return _fade_overlap;
}

void CrossFader::set_fade_overlap(quint64 fade_overlap)
{
    _fade_overlap = fade_overlap;
}

quint64 CrossFader::get_fading_time() const
{
    return _fading_time;
}

void CrossFader::set_fading_time(quint64 fading_time)
{
    _fading_time = fading_time;
}


void CrossFader::abort_fader()
{

    if(_fader && _fader_data->cycles > 0){
	_fader_data->cycles = 0;
	/*if(_fader->joinable()){
	    _fader->join();
	}*/
    }
}


