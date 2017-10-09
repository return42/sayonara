#ifndef SPEEDHANDLER_H
#define SPEEDHANDLER_H

#include "Helper/Pimpl.h"
#include <gst/gst.h>

class SpeedHandler
{
	PIMPL(SpeedHandler)

public:
	SpeedHandler();
	virtual ~SpeedHandler();

	void set_speed(float speed, double pitch, bool preserve_pitch);
	virtual GstElement* get_pitch_element() const=0;
};

#endif // SPEEDHANDLER_H
