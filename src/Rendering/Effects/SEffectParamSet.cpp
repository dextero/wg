#include "SEffectParamSet.h"
#include "../ZIndexVals.h"

SEffectParamSet::SEffectParamSet():
    rotation(0.f),
    amount(0.f),
    offsetX(0.f),
    offsetY(0.f)
{
    zIndex = Z_PARTICLE;
    duration = 1.0;
    scale = 1.0;
    red = 1.0;
    green = 1.0;
    blue = 1.0;
    alpha = 1.0;
    removeTogetherWithTraced = false;
}

