#include "SeekHandler.h"




struct SeekHandler::Private
{
	static const GstSeekFlags SeekAccurate=(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE);
	static const GstSeekFlags SeekNearest=(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE);

	bool seek(GstElement* audio_src, GstSeekFlags flags, gint64 ns)
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

	bool seek_accurate(GstElement* audio_src, gint64 ns)
	{
		return seek(audio_src, SeekAccurate, ns);
	}

	bool seek_nearest(GstElement* audio_src, gint64 ns)
	{
		return seek(audio_src, SeekNearest, ns);
	}
};

SeekHandler::SeekHandler()
{
	m = Pimpl::make<Private>();
}

SeekHandler::~SeekHandler() {}

gint64 SeekHandler::seek_rel(double percent, gint64 ref_ns)
{
	gint64 new_time_ns;

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


gint64 SeekHandler::seek_abs(gint64 ns)
{
	ns = std::max((gint64) 0, ns);

	if( m->seek_accurate(get_source(), ns) ) {
		return ns;
	}

	return 0;
}

gint64 SeekHandler::seek_nearest(gint64 ns)
{
	ns = std::max((gint64) 0, ns);

	if( m->seek_nearest(get_source(), ns) ) {
		return ns;
	}

	return 0;
}
