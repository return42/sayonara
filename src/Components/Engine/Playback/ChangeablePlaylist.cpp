#include "ChangeablePlaylist.h"
#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"

struct ProbeData
{
    GstElement* first_element;
    GstElement* second_element;
    GstElement* element_of_interest;
    GstElement* pipeline;
    GstState old_state;
    bool done;

    ProbeData()
    {
	done = false;
    }
};

ChangeablePlaylist::ChangeablePlaylist()
{

}

static GstPadProbeReturn
eos_probe_installed_add (GstPad* pad, GstPadProbeInfo * info, gpointer data)
{
    ProbeData* probe_data = (ProbeData*) data;

    if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) != GST_EVENT_EOS){
	return GST_PAD_PROBE_PASS;
    }

    gst_pad_remove_probe (pad, GST_PAD_PROBE_INFO_ID (info));

    gst_element_set_state (probe_data->second_element, GST_STATE_NULL);

    gst_bin_add (GST_BIN (probe_data->pipeline), probe_data->element_of_interest);

    gst_element_unlink(probe_data->first_element,
		       probe_data->second_element);

    gst_element_link_many (probe_data->first_element,
			   probe_data->element_of_interest,
			   probe_data->second_element, NULL);

    /*gst_element_set_state (probe_data->element_of_interest, probe_data->old_state);
    gst_element_set_state (probe_data->second_element, probe_data->old_state);*/
    gst_element_set_state (probe_data->pipeline, probe_data->old_state);

    probe_data->done = true;

    return GST_PAD_PROBE_DROP;
}



static GstPadProbeReturn
src_blocked_add(GstPad* pad, GstPadProbeInfo* info, gpointer data)
{
    GstPad *srcpad, *sinkpad;
    ProbeData* probe_data = (ProbeData*) data;
    gst_pad_remove_probe (pad, GST_PAD_PROBE_INFO_ID (info));

    srcpad = gst_element_get_static_pad(probe_data->second_element, "src");
    sp_log(Log::Debug) << "Try to add probe to pad: " << gst_element_get_name(probe_data->second_element);

    gst_pad_add_probe (srcpad,
		       (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM),
		       eos_probe_installed_add,
		       probe_data,
		       NULL);

    gst_object_unref (srcpad);

    sinkpad = gst_element_get_static_pad (probe_data->second_element, "sink");
    gst_pad_send_event (sinkpad, gst_event_new_eos ());
    gst_object_unref (sinkpad);

    return GST_PAD_PROBE_OK;
}


void ChangeablePlaylist::add_element(GstElement* element, GstElement* first_element, GstElement* second_element)
{
    GstElement* pipeline = get_pipeline();
    gchar* element_name = gst_element_get_name(element);
    sp_log(Log::Debug) << "Add " << element_name << " to pipeline";
    if(gst_bin_get_by_name((GstBin*)pipeline, element_name) != nullptr){
	sp_log(Log::Debug) << "Element already in pipeline";
	return;
    }

    GstPad* pad = gst_element_get_static_pad(first_element, "src");
    ProbeData* data = new ProbeData();
	data->first_element = first_element;
	data->second_element = second_element;
	data->element_of_interest = element;
	data->pipeline = pipeline;

    gst_element_get_state(pipeline, &data->old_state, nullptr, 0);

    if(data->old_state == GST_STATE_NULL){
	gst_element_unlink(data->first_element, data->second_element);
	gst_bin_add((GstBin*)pipeline, data->element_of_interest);
	gst_element_link_many(data->first_element, data->element_of_interest, data->second_element, nullptr);
	return;
    }

    gulong id = gst_pad_add_probe (pad,
		       GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM,
		       src_blocked_add,
		       data,
		       nullptr);

    sp_log(Log::Debug) << "Probe id = " << id;

    while(!data->done)
    {
	Helper::sleep_ms(100);
    }

    sp_log(Log::Debug) << "Element " << gst_element_get_name(element) << " added.";
    delete data;
}


static GstPadProbeReturn
eos_probe_installed_remove (GstPad* pad, GstPadProbeInfo * info, gpointer data)
{
    ProbeData* probe_data = (ProbeData*) data;

    sp_log(Log::Debug) << "Eos probe installed remove";

    if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) != GST_EVENT_EOS)
	return GST_PAD_PROBE_PASS;

    gst_pad_remove_probe (pad, GST_PAD_PROBE_INFO_ID (info));

    gst_element_set_state (probe_data->element_of_interest, GST_STATE_NULL);


    gst_element_set_state (probe_data->element_of_interest, GST_STATE_NULL);


    gst_element_unlink_many (probe_data->first_element,
			   probe_data->element_of_interest,
			   probe_data->second_element, NULL);

    gst_bin_remove (GST_BIN(probe_data->pipeline), probe_data->element_of_interest);

    gst_element_link(probe_data->first_element,
		       probe_data->second_element);

    gst_element_set_state (probe_data->second_element, probe_data->old_state);
    gst_element_set_state (probe_data->pipeline, probe_data->old_state);
    probe_data->done = true;

    return GST_PAD_PROBE_DROP;
}



static GstPadProbeReturn
src_blocked_remove(GstPad* pad, GstPadProbeInfo* info, gpointer data)
{
    sp_log(Log::Debug) << "Source pad probed (Remove)";
    GstPad *srcpad, *sinkpad;
    ProbeData* probe_data = (ProbeData*) data;

    gst_pad_remove_probe (pad, GST_PAD_PROBE_INFO_ID (info));

    srcpad = gst_element_get_static_pad(probe_data->element_of_interest, "src");
    gst_pad_add_probe (srcpad,
		       (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM),
		       eos_probe_installed_remove,
		       probe_data,
		       NULL);

    gst_object_unref (srcpad);

    sinkpad = gst_element_get_static_pad (probe_data->element_of_interest, "sink");
    gst_pad_send_event (sinkpad, gst_event_new_eos ());
    gst_object_unref (sinkpad);

    return GST_PAD_PROBE_OK;
}

void ChangeablePlaylist::remove_element(GstElement* element, GstElement* first_element, GstElement* second_element)
{
    GstElement* pipeline = get_pipeline();

    char* element_name = gst_element_get_name(element);
    sp_log(Log::Debug) << "Looking for element " << element_name;

    if(!gst_bin_get_by_name((GstBin*)pipeline, element_name)){
	sp_log(Log::Debug) << "Element not there";
	return;
    }

    sp_log(Log::Debug) << "Found element in pipeline";

    GstPad* pad = gst_element_get_static_pad(first_element, "src");

    ProbeData* data = new ProbeData();
	data->first_element = first_element;
	data->second_element = second_element;
	data->element_of_interest = element;
	data->pipeline = pipeline;
    gst_element_get_state(pipeline, &data->old_state, nullptr, 0);

    if(data->old_state == GST_STATE_NULL)
    {
	gst_element_unlink_many(first_element, element, second_element, nullptr);
	gst_bin_remove((GstBin*) pipeline, element);
	gst_element_link(first_element, second_element);
	return;
    }

    gulong id = gst_pad_add_probe (pad,
		       GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM,
		       src_blocked_remove,
		       data,
		       nullptr);

    sp_log(Log::Debug) << "Probe id = " << id;


    while(!data->done)
    {
	//sp_log(Log::Debug) << "Waiting for removal";
	Helper::sleep_ms(100);
    }

    sp_log(Log::Debug) << "Element " << gst_element_get_name(element) << " removed.";

    delete data;
}



