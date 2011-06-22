#ifndef __SANIMATION__
#define __SANIMATION__

#include "STimedAnimationFrame.h"
#include <vector>
#include <string>

struct SAnimation
{
    std::vector< STimedAnimationFrame > frames;

    float TotalLength(); 

    std::string name;
};

#endif// __SANIMATION__

