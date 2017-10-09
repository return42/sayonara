#ifndef CROSSFADERENGINE_H
#define CROSSFADERENGINE_H

#include <gst/gst.h>
class Engine;

class CrossfaderEngine
{
public:
	CrossfaderEngine(Engine* parent=nullptr);


	void set_about_to_finish(int64_t time2go);
	void set_track_finished();

	GaplessState gapless_state() const;
};

#endif // CROSSFADERENGINE_H
