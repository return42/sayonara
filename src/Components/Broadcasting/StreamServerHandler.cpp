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
    Set::listen(Set::Broadcast_Active, this, &StreamServerHandler::active_changed);

    m = Pimpl::make<Private>();
    m->stream_server = new StreamServer();
}

StreamServerHandler::~StreamServerHandler() {}

void StreamServerHandler::active_changed()
{
    if( _settings->get(Set::Broadcast_Active) &&
        _settings->get(SetNoDB::MP3enc_found))
    {
        if(!!m->stream_server->isRunning())
        {
            m->stream_server->start();
        }
    }

    else{
        m->stream_server->stop();
    }
}
