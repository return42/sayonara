#ifndef STREAMRECORDERHANDLER_H
#define STREAMRECORDERHANDLER_H

#include "Utils/Pimpl.h"
#include "Components/Engine/gstfwd.h"

class QString;

namespace StreamRecorder
{
    struct Data;
}

namespace Pipeline
{
    class StreamRecorderHandler
    {
        PIMPL(StreamRecorderHandler)

    public:
        StreamRecorderHandler();
        virtual ~StreamRecorderHandler();

        void set_streamrecorder_target_path(const QString& path);
        StreamRecorder::Data* streamrecorder_data() const;

        virtual GstElement* get_streamrecorder_sink_element() const=0;
    };
}

#endif // STREAMRECORDERHANDLER_H
