#ifndef SEEKHANDLER_H
#define SEEKHANDLER_H

#include "Helper/Pimpl.h"
#include <gst/gst.h>

class SeekHandler
{
	PIMPL(SeekHandler)

public:
	SeekHandler();
	virtual ~SeekHandler();

	gint64 seek_rel(double percent, gint64 ref_ns);
	gint64 seek_abs(gint64 ns);
	gint64 seek_nearest(gint64 ns);

	virtual GstElement* get_source() const=0;
};

#endif // SEEKHANDLER_H
