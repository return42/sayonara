/* CrossFader.cpp */

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


/* CrossFader.cpp */

#include "Crossfader.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Utils.h"
#include "Utils/Logger/Logger.h"

#include <QThread>

#include <glib.h>
#include <functional>

using Pipeline::CrossFader;

class FaderThreadData
{
private:

	int						_cycles;
	int						_cycle_time_ms;
	std::function<void ()>	_fn;

public:

	FaderThreadData(const std::function<void ()>& fn){
		reset();
		_fn = fn;
	}

	void set_fading_time(int fading_time){
		_cycle_time_ms = fading_time / _cycles;
	}

	void reset()
	{
		_cycles = get_max_cycles();
	}

	bool is_active() const
	{
		return (_cycles > 0);
	}

	void abort()
	{
		_cycles = 0;
	}

	void wait()
	{
		Util::sleep_ms(_cycle_time_ms);
		_cycles --;
		_fn();
	}

	int get_cycles() const
	{
		return _cycles;
	}

	static int get_max_cycles()
	{
		return 500;
	}
};

class FaderThread : public QThread
{

private:
	FaderThreadData* _ftd=nullptr;

public:
	FaderThread(FaderThreadData* data) :
		QThread(nullptr)
	{
		_ftd = data;
	}

protected:
	void run() override
	{
		while(_ftd && _ftd->is_active())
		{
			_ftd->wait();
		}
	}
};

struct CrossFader::Private
{
	FaderThread*    fader=nullptr;
	FaderThreadData* fader_data=nullptr;

	double			volume;
	double			fade_step;

	FadeMode	    fade_mode;

	Private() :
		volume(0),
		fade_step(0),
		fade_mode(CrossFader::FadeMode::NoFading)
	{}
};

CrossFader::CrossFader()
{
	m = Pimpl::make<Private>();

	m->fader_data = new FaderThreadData(
		std::bind(&CrossFader::fader_timed_out, this)
	);
}

CrossFader::~CrossFader() {}

void CrossFader::init_fader()
{
	if(m->fade_mode == CrossFader::FadeMode::NoFading){
		return;
	}

	if(m->fader && m->fader_data->is_active())
	{
		m->fader_data->abort();

		while(m->fader->isRunning()){
			Util::sleep_ms(10);
		}

		delete m->fader; m->fader=nullptr;
	}

	int fading_time = Settings::instance()->get(Set::Engine_CrossFaderTime);

	m->fader_data->reset();
	m->fader_data->set_fading_time(fading_time);
	m->fader = new FaderThread(m->fader_data);
	m->fader->start();
}



void CrossFader::fade_in()
{
	double volume = Settings::instance()->get(Set::Engine_Vol) / 100.0;

	m->volume = 0;
	m->fade_mode = CrossFader::FadeMode::FadeIn;
	m->fade_step = volume / (FaderThreadData::get_max_cycles() * 1.0);

	set_current_volume(0.00001);

	init_fader();
	play();
}

void CrossFader::fade_out()
{
	m->volume = Settings::instance()->get(Set::Engine_Vol) / 100.0;
	m->fade_mode = CrossFader::FadeMode::FadeOut;
	m->fade_step = m->volume / (FaderThreadData::get_max_cycles() * 1.0);

	set_current_volume( m->volume );

	init_fader();
}

bool CrossFader::is_fading_out() const
{
	return (m->fader &&
			m->fader->isRunning() &&
			(m->fade_mode == CrossFader::FadeMode::FadeOut));
}

bool CrossFader::is_fading_int() const
{
	return (m->fader &&
			m->fader->isRunning() &&
			(m->fade_mode == CrossFader::FadeMode::FadeIn));
}


void CrossFader::fader_timed_out()
{
	if(m->fade_mode == CrossFader::FadeMode::FadeIn){
		increase_volume();
	}

	else if(m->fade_mode == CrossFader::FadeMode::FadeOut){
		decrease_volume();
	}
}


void CrossFader::increase_volume()
{
	double max_volume = Settings::instance()->get(Set::Engine_Vol) / 100.0;

	// maybe volume has changed in the meantime
	m->fade_step = std::max(m->fade_step, m->volume / (m->fader_data->get_cycles() * 1.0));
	m->volume += m->fade_step;

	if(m->volume > max_volume)
	{
		abort_fader();
		return;
	}

	set_current_volume(m->volume);
}


void CrossFader::decrease_volume()
{
	double max_volume = Settings::instance()->get(Set::Engine_Vol) / 100.0;

	// maybe volume has changed in the meantime
	m->volume = std::min(m->volume, max_volume);
	m->fade_step = std::max(m->fade_step, m->volume / (m->fader_data->get_cycles() * 1.0));

	m->volume -= m->fade_step;

	if(m->volume < 0.00001){
		abort_fader();
		stop();
		return;
	}

	set_current_volume(m->volume);
}

uint64_t CrossFader::get_fading_time_ms() const
{
	Settings* settings = Settings::instance();
	if(settings->get(Set::Engine_CrossFaderActive)){
		return settings->get(Set::Engine_CrossFaderTime);
	}

	return 0;
}

void CrossFader::abort_fader()
{
	if(m->fader_data->is_active()){
		m->fader_data->abort();
	}
}


