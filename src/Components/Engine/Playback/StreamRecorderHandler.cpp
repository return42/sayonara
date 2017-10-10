#include "StreamRecorderHandler.h"
#include "PipelineProbes.h"
#include "StreamRecorderData.h"

#include "Utils/Settings/Settings.h"
#include <QString>

struct StreamRecorderHandler::Private
{
	Settings*			settings=nullptr;
	QString				sr_path;
	bool				run_sr;
	StreamRecorderData* sr_data=nullptr;

	Private() :
		sr_data(new StreamRecorderData())
	{
		settings = Settings::instance();
	}
};

StreamRecorderHandler::StreamRecorderHandler()
{
	m = Pimpl::make<Private>();
}

StreamRecorderHandler::~StreamRecorderHandler() {}

StreamRecorderData*StreamRecorderHandler::streamrecorder_data() const
{
	return m->sr_data;
}

void StreamRecorderHandler::set_streamrecorder_target_path(const QString& path)
{
	GstElement* file_sink_element = get_streamrecorder_sink_element();
	if(!file_sink_element) {
		return;
	}

	if(path == m->sr_path && !m->sr_path.isEmpty()) {
		return;
	}

	if(m->sr_data->busy){
		return;
	}

	m->sr_path = path;
	m->run_sr = !(path.isEmpty());

	gchar* old_filename = m->sr_data->filename;

	m->sr_data->filename = strdup(m->sr_path.toUtf8().data());
	m->sr_data->active = m->run_sr;

	Probing::handle_stream_recorder_probe(m->sr_data, Probing::stream_recorder_probed);

	if(old_filename){
		free(old_filename);
	}
}

