#ifndef __STIMED_ANIMATION_FRAME__
#define __STIMED_ANIMATION_FRAME__

#include "SAnimationFrame.h"

struct STimedAnimationFrame
{
    STimedAnimationFrame( float _time, const SAnimationFrame& _frame ) :
        time( _time ), frame( _frame ) {};

    float time;
    SAnimationFrame frame;
};

#endif// __STIMED_ANIMATION_FRAME__

