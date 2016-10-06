#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <QtGlobal>

enum class PlayState : quint8
{
    Playing=0,
    Paused,
    Stopped
};

#endif // PLAYSTATE_H
