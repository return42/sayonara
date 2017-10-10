#include "EqualizerHandler.h"
#include "Utils/Settings/Settings.h"
#include "Utils/EqualizerPresets.h"
#include "Utils/globals.h"

#include <QString>
#include <gst/gst.h>

using Pipeline::EqualizerHandler;

struct EqualizerHandler::Private
{
	Settings* settings=nullptr;
	Private()
	{
		settings = Settings::instance();
	}
};

EqualizerHandler::EqualizerHandler()
{
	m = Pimpl::make<Private>();
}


EqualizerHandler::~EqualizerHandler() {}

void EqualizerHandler::init_equalizer()
{
	int last_idx = m->settings->get(Set::Eq_Last);
	QList<EQ_Setting> presets = m->settings->get(Set::Eq_List);
	presets.push_front(EQ_Setting());

	if( !between(last_idx, presets)){
		last_idx = 0;
	}

	EQ_Setting last_preset = presets[last_idx];
	QList<int> values = last_preset.values();

	for(int i=0; i<values.size(); i++)
	{
		set_band(i, values[i]);
	}
}

void EqualizerHandler::set_band(int band, int val)
{
	QString band_name = QString("band%1").arg(band);

	GstElement* equalizer_element = get_equalizer_element();
	if(!equalizer_element){
		return;
	}

	double new_val;
	if (val > 0) {
		new_val = val * 0.25;
	}

	else{
		new_val = val * 0.75;
	}

	g_object_set( G_OBJECT(equalizer_element),
	              band_name.toUtf8().data(),
                  new_val,
	              nullptr
	);
}

