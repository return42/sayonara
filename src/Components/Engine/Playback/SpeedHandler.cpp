#include "SpeedHandler.h"
#include "Helper/Settings/Settings.h"

struct SpeedHandler::Private
{
	Settings* settings=nullptr;

	Private()
	{
		settings = Settings::instance();
	}
};

SpeedHandler::SpeedHandler()
{
	m = Pimpl::make<Private>();
}

SpeedHandler::~SpeedHandler() {}

void SpeedHandler::set_speed(float speed, double pitch, bool preserve_pitch)
{
	if(!m->settings->get(Set::Engine_SpeedActive))
	{
		return;
	}

	GstElement* pitch_element = get_pitch_element();

	if(!pitch_element){
		return;
	}

	if(preserve_pitch){
		g_object_set(pitch_element,
		             "tempo", speed,
		             "rate", 1.0,
		             "pitch", pitch,
		             nullptr);
	}

	else{
		g_object_set(pitch_element,
		             "tempo", 1.0,
		             "rate", speed,
		             "pitch", pitch,
		             nullptr);
	}
}
