#ifndef STREAMRECORDERHANDLER_H
#define STREAMRECORDERHANDLER_H

#include "Utils/Pimpl.h"

struct _GstElement;
typedef struct _GstElement GstElement;

class QString;
struct StreamRecorderData;

class StreamRecorderHandler
{
	PIMPL(StreamRecorderHandler)
public:
	StreamRecorderHandler();
	virtual ~StreamRecorderHandler();

	void set_streamrecorder_target_path(const QString& path);
	StreamRecorderData* streamrecorder_data() const;

	virtual GstElement* get_streamrecorder_sink_element() const=0;
};

#endif // STREAMRECORDERHANDLER_H
