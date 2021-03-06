/* GSTPipeline.cpp */

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

#include "AbstractPipeline.h"
#include "Components/Engine/AbstractEngine.h"

#include "Callbacks/EngineCallbacks.h"
#include "Callbacks/PipelineCallbacks.h"
#include "Utils/Logger/Logger.h"

#include <gst/app/gstappsink.h>

using Pipeline::Base;
using Pipeline::test_and_error;
using Pipeline::test_and_error_bool;

struct Base::Private
{
    Engine::Base*   engine=nullptr;
    QTimer*         progress_timer=nullptr;
    bool            initialized;

    Private(Engine::Base* engine) :
        engine(engine),
        initialized(false)
    {}

    int64_t query_duration(GstElement* source) const
    {
        int64_t duration_ns;

        bool success = gst_element_query_duration(source, GST_FORMAT_TIME, &duration_ns);
        if(success){
            return GST_TIME_AS_MSECONDS(duration_ns);
        }

        return 0;
    }
};

Base::Base(QString name, Engine::Base* engine, QObject* parent) :
	QObject(parent),
	SayonaraClass()
{
    m = Pimpl::make<Private>(engine);

	_name = name;
	_position_source_ms = 0;
	_position_pipeline_ms = 0;
    _duration_ms = 0;
	_about_to_finish = false;
}

Base::~Base()
{
	if (_bus){
		gst_object_unref (_bus);
	}

	if (_pipeline) {
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
		gst_object_unref (GST_OBJECT(_pipeline));
	}
}


bool Base::init(GstState state)
{
	bool success = false;
    if(m->initialized) {
		return true;
	}

	// create equalizer element
	_pipeline = gst_pipeline_new(_name.toStdString().c_str());
    if(!test_and_error(_pipeline, "Engine: Pipeline sucks")){
		return false;
	}

	_bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));

	success = create_elements();
	if(!success) {
		return false;
	}

	success = add_and_link_elements();
	if(!success) {
		return false;
	}

	configure_elements();

	gst_element_set_state(_pipeline, state);

#ifdef Q_OS_WIN
    gst_bus_set_sync_handler(_bus, Engine::Callbacks::bus_message_received, m->engine, EngineCallbacks::destroy_notify);
#else
    gst_bus_add_watch(_bus, Engine::Callbacks::bus_state_changed, m->engine);
#endif

    m->progress_timer = new QTimer(this);
    m->progress_timer->setInterval(200);
    connect(m->progress_timer, &QTimer::timeout, [=]()
	{
		if(this->get_state() != GST_STATE_NULL){
            Callbacks::position_changed(this);
		}
	});

    m->progress_timer->start();

    m->initialized = true;
	return true;
}

void Base::refresh_position() 
{
    int64_t pos_pipeline, pos_source;
	bool success_source, success_pipeline;
	GstElement* element;

	element = get_source();

	if(!element){
		element = GST_ELEMENT(_pipeline);
	}

	success_source = gst_element_query_position(element, GST_FORMAT_TIME, &pos_source);
	success_pipeline = gst_element_query_position(_pipeline, GST_FORMAT_TIME, &pos_pipeline);

	_position_source_ms = 0;
	_position_pipeline_ms = 0;
	if(success_source) {
		_position_source_ms = GST_TIME_AS_MSECONDS(pos_source);
	}

	if(success_pipeline) {
		_position_pipeline_ms = GST_TIME_AS_MSECONDS(pos_pipeline);
	}

	if(_duration_ms >= 0) {
		emit sig_pos_changed_ms( _position_pipeline_ms );
	}
}

void Base::update_duration_ms(int64_t duration_ms, GstElement *src)
{
    if(src == get_source()){
        _duration_ms = duration_ms;
    }

    refresh_position();
}

void Base::set_data(uchar* data, uint64_t size){
	emit sig_data(data, size);
}


//static void show_time_info(int64_t pos, int64_t dur){
//	sp_log(Log::Develop, this) << "Difference: "
//					   << dur - pos << ": "
//					   << pos << " - "
//					   << dur;
//}


void Base::check_about_to_finish()
{
    int64_t difference = _duration_ms - _position_pipeline_ms;

    if(difference <= 0 && !_about_to_finish)
    {
        _duration_ms = m->query_duration(get_source());

		if(_duration_ms <= 0){
			return;
		}

		difference = _duration_ms - _position_pipeline_ms;
	}

	//show_time_info(_position_ms, _duration_ms);

	int64_t about_to_finish_time = (int64_t) get_about_to_finish_time();

	if(difference < about_to_finish_time && !_about_to_finish) {
		_about_to_finish = true;
		emit sig_about_to_finish(difference);
	}

	else if(difference > about_to_finish_time){
		_about_to_finish = false;
	}
}

int64_t Base::get_time_to_go() const
{
    int64_t position, duration;
	GstElement* element;

	element = get_source();
	if(!element){
		element = GST_ELEMENT(_pipeline);
	}

	element = GST_ELEMENT(_pipeline);

	gst_element_query_duration(element, GST_FORMAT_TIME, &duration);
	gst_element_query_position(element, GST_FORMAT_TIME, &position);

	return GST_TIME_AS_MSECONDS(duration - position) - 100;
}


int64_t Base::get_duration_ms() const
{
    return _duration_ms;
}

int64_t Base::get_source_position_ms() const
{
	return _position_source_ms;
}

int64_t Base::get_pipeline_position_ms() const
{
	return _position_pipeline_ms;
}

void Base::finished()
{
	emit sig_finished();
}


GstState Base::get_state()
{
	GstState state;
	gst_element_get_state(_pipeline, &state, nullptr, GST_MSECOND * 10);
	return state;
}


GstElement* Base::get_pipeline() const
{
	return _pipeline;
}

bool Base::set_uri(gchar* uri)
{
	_uri = uri;
    return (_uri != nullptr);
}



bool Base::create_element(GstElement** elem, const gchar* elem_name, const gchar* name)
{
	QString error_msg;
	if(strlen(name) > 0){
		*elem = gst_element_factory_make(elem_name, name);
		error_msg = QString("Engine: ") + name + " creation failed";
	}

	else{
		*elem = gst_element_factory_make(elem_name, elem_name);
		error_msg = QString("Engine: ") + elem_name + " creation failed";
	}

    bool success = test_and_error(*elem, error_msg);

	return success;
}


bool Base::tee_connect(GstElement* tee, GstPadTemplate* tee_src_pad_template, GstElement* queue, const QString& queue_name)
{
	GstPadLinkReturn s;
	GstPad* tee_queue_pad;
	GstPad* queue_pad;

	QString error_1 = QString("Engine: Tee-") + queue_name + " pad is nullptr";
	QString error_2 = QString("Engine: ") + queue_name + " pad is nullptr";
	QString error_3 = QString("Engine: Cannot link tee with ") + queue_name;

	tee_queue_pad = gst_element_request_pad(tee, tee_src_pad_template, nullptr, nullptr);
    if(!test_and_error(tee_queue_pad, error_1)){
		return false;
	}

	queue_pad = gst_element_get_static_pad(queue, "sink");
    if(!test_and_error(queue_pad, error_2)) {
		return false;
	}

	s = gst_pad_link (tee_queue_pad, queue_pad);
    if(!test_and_error_bool((s == GST_PAD_LINK_OK), error_3)) {
		return false;
	}

	g_object_set (queue, "silent", TRUE, nullptr);

	gst_object_unref(tee_queue_pad);
	gst_object_unref(queue_pad);
	return true;
}


bool
Base::has_element(GstElement* e) const
{
	if(!e){
		return true;
	}

	GstObject* o = (GstObject*) e;
	GstObject* parent = nullptr;

	while(o){
		if( o == (GstObject*) _pipeline ){
			if( (GstObject*) e != o ){
				gst_object_unref(o);
			}

			return true;
		}

		parent = gst_object_get_parent(o);
		if( (GstObject*) e != o ){
			gst_object_unref(o);
		}

		o = parent;
	}

	return false;
}


uint64_t Base::get_about_to_finish_time() const
{
	return 300;
}

void Base::play()
{
	GstElement* pipeline = get_pipeline();
	if(pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
	}
}

void Base::pause()
{
	GstElement* pipeline = get_pipeline();
	if(pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_PAUSED);
	}
}

void Base::stop()
{
	GstElement* pipeline = get_pipeline();
	if(pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_NULL);
	}

	_position_source_ms = 0;
	_position_pipeline_ms = 0;
	_duration_ms = 0;
	_uri = nullptr;
}

bool
Pipeline::test_and_error(void* element, const QString& errorstr)
{
	if(!element) {
		sp_log(Log::Error) << errorstr;
		return false;
	}

	return true;
}

bool
Pipeline::test_and_error_bool(bool b, const QString& errorstr)
{
	if(!b) {
		sp_log(Log::Error) << errorstr;
		return false;
	}

	return true;
}
