#include "StreamServerHandler.h"
#include "StreamServer.h"
#include "Utils/Settings/Settings.h"

struct StreamServerHandler::Private
{
	StreamServer* stream_server=nullptr;

	Private() {}
	~Private()
	{
		if(stream_server){
			delete stream_server;
		}
	}
};

StreamServerHandler::StreamServerHandler()
{
	m = Pimpl::make<Private>();

	m->stream_server = new StreamServer();
}

StreamServerHandler::~StreamServerHandler() {}

void StreamServerHandler::active_changed()
{

}

void StreamServerHandler::port_changed()
{
	m->stream_server->restart();
}
