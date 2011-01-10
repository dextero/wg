#include "SAnimation.h"
#include "STimedAnimationFrame.h"

float SAnimation::TotalLength()
{
    if (frames.empty()) 
        return 0.0f;
    else
       return frames.back().time;
}

