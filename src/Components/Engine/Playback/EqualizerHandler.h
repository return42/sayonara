#ifndef EQUALIZERHANDLER_H
#define EQUALIZERHANDLER_H

#include "Helper/Pimpl.h"

struct _GstElement;
typedef struct _GstElement GstElement;

class QString;

class EqualizerHandler
{
	PIMPL(EqualizerHandler)

public:
	EqualizerHandler();
	virtual ~EqualizerHandler();

	void init_equalizer();
	void set_band(int, int val);

	virtual GstElement* get_equalizer_element() const=0;
};

#endif // EQUALIZERHANDLER_H
