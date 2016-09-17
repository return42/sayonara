#ifndef CHANGEABLEPLAYLIST_H
#define CHANGEABLEPLAYLIST_H

#include "gst/gst.h"
class ChangeablePlaylist
{
public:
	ChangeablePlaylist();

	void add_element(GstElement* element, GstElement* first_element, GstElement* second_element);
	void remove_element(GstElement* element, GstElement* first_element, GstElement* second_element);

protected:
	virtual GstElement* get_pipeline() const=0;
};

#endif // CHANGEABLEPLAYLIST_H
