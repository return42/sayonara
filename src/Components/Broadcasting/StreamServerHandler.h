#ifndef STREAMSERVERHANDLER_H
#define STREAMSERVERHANDLER_H

#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Pimpl.h"

class StreamServerHandler : public SayonaraClass
{
    PIMPL(StreamServerHandler)

public:
    StreamServerHandler();
    ~StreamServerHandler();

    void active_changed();
};

#endif // STREAMSERVERHANDLER_H
