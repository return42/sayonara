#ifndef SPEEDHANDLER_H
#define SPEEDHANDLER_H

#include "Utils/Pimpl.h"
#include "Components/Engine/gstfwd.h"

namespace Pipeline
{
    class SpeedHandler
    {
        PIMPL(SpeedHandler)

    public:
        SpeedHandler();
        virtual ~SpeedHandler();

        void set_speed(float speed, double pitch, bool preserve_pitch);
        virtual GstElement* get_pitch_element() const=0;
    };
}

#endif // SPEEDHANDLER_H
