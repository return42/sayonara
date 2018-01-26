#include "DelayedPlayHandler.h"
#include "Utils/Utils.h"
#include <QTimer>

using Pipeline::DelayedPlayHandler;
using Pipeline::Logic;

struct Logic::Private
{
	Pipeline::DelayedPlayHandler* dph=nullptr;
	QTimer* t=nullptr;

	Private(Pipeline::DelayedPlayHandler* dph) :
		dph(dph)
	{
		t = new QTimer();
		t->setTimerType(Qt::PreciseTimer);
		t->setSingleShot(true);
	}

	~Private()
	{
		while(t->isActive())
		{
			t->stop();
			::Util::sleep_ms(100);
		}

		delete t; t = nullptr;
	}
};

Logic::Logic(Pipeline::DelayedPlayHandler* dph)
{
	m = Pimpl::make<Private>(dph);

	connect(m->t, &QTimer::timeout, [=](){
		m->dph->play();
	});
}

Logic::~Logic() {}

void Logic::start_timer(MilliSeconds ms)
{
	m->t->start(ms);
}

void Logic::stop_timer()
{
	m->t->stop();
}

struct DelayedPlayHandler::Private
{
	Logic* logic=nullptr;

	Private(DelayedPlayHandler* dph)
	{
		logic = new Logic(dph);
	}

	~Private()
	{
		delete logic; logic = nullptr;
	}
};

DelayedPlayHandler::DelayedPlayHandler()
{
	m = Pimpl::make<Private>(this);
}

DelayedPlayHandler::~DelayedPlayHandler() {}

void DelayedPlayHandler::play_in(MilliSeconds ms)
{
	abort_delayed_playing();

	if(ms <= 0){
		play();
	}

	else {
		m->logic->start_timer(ms);
	}
}

void DelayedPlayHandler::abort_delayed_playing()
{
	m->logic->stop_timer();
}

