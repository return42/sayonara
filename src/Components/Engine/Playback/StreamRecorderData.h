#ifndef STREAMRECORDERDATA_H
#define STREAMRECORDERDATA_H

#include <gst/gst.h>

/**
 * @brief The StreamRecorderData struct
 * @ingroup Engine
 */
struct StreamRecorderData
{
	GstElement* queue=nullptr;
	GstElement* sink=nullptr;
	gchar* filename=nullptr;

	bool active;
	int probe_id;
	bool busy;
	bool has_empty_filename;

	StreamRecorderData();
	~StreamRecorderData();

};

#endif // STREAMRECORDERDATA_H
