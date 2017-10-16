/* SeekHandler.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "SeekHandler.h"
#include <gst/gst.h>

using Pipeline::SeekHandler;

struct SeekHandler::Private
{
	static const GstSeekFlags SeekAccurate=(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE);
	static const GstSeekFlags SeekNearest=(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE);

    bool seek(GstElement* audio_src, GstSeekFlags flags, int64_t ns)
	{
		if(!audio_src){
			return false;
		}

		return gst_element_seek_simple (
		            audio_src,
		            GST_FORMAT_TIME,
		            flags,
		            ns);
	}

    bool seek_accurate(GstElement* audio_src, int64_t ns)
	{
		return seek(audio_src, SeekAccurate, ns);
	}

    bool seek_nearest(GstElement* audio_src, int64_t ns)
	{
		return seek(audio_src, SeekNearest, ns);
	}
};

SeekHandler::SeekHandler()
{
	m = Pimpl::make<Private>();
}

SeekHandler::~SeekHandler() {}

int64_t SeekHandler::seek_rel(double percent, int64_t ref_ns)
{
    int64_t new_time_ns;

	if (percent > 1.0){
		new_time_ns = ref_ns;
	}

	else if(percent < 0){
		new_time_ns = 0;
	}

	else{
		new_time_ns = (percent * ref_ns); // nsecs
	}


	if( m->seek_accurate(get_source(), new_time_ns) ) {
		return new_time_ns;
	}

	return 0;
}


int64_t SeekHandler::seek_abs(int64_t ns)
{
    ns = std::max((int64_t) 0, ns);

	if( m->seek_accurate(get_source(), ns) ) {
		return ns;
	}

	return 0;
}

int64_t SeekHandler::seek_nearest(int64_t ns)
{
    ns = std::max((int64_t) 0, ns);

	if( m->seek_nearest(get_source(), ns) ) {
		return ns;
	}

	return 0;
}
