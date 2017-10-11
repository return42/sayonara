#ifndef SEEKHANDLER_H
#define SEEKHANDLER_H

#include "Utils/Pimpl.h"
#include "Components/Engine/gstfwd.h"

namespace Pipeline
{
    class SeekHandler
    {
        PIMPL(SeekHandler)

    public:
        SeekHandler();
        virtual ~SeekHandler();

        int64_t seek_rel(double percent, int64_t ref_ns);
        int64_t seek_abs(int64_t ns);
        int64_t seek_nearest(int64_t ns);

        virtual GstElement* get_source() const=0;
    };
}

#endif // SEEKHANDLER_H
