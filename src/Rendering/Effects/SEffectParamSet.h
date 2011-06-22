#ifndef __EFFECT_PARAM_SET_H__
#define __EFFECT_PARAM_SET_H__

#include <vector>
#include <string>

struct SEffectParamSet
{
    SEffectParamSet();

    std::string name;
    int zIndex;
    std::string image;
    std::string animation;
    std::string particle;
    std::string effectAnimation;
    float duration;
    float scale;
    float rotation;
    float amount;

	// do piorunow
	float offsetX;
	float offsetY;
    
// "colorize"
    float red;
    float green;
    float blue;
    float alpha;

    bool removeTogetherWithTraced;

};



#endif//__EFFECT_PARAM_SET_H__

