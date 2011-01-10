#ifndef __CANIMATION_STATE_H__
#define __CANIMATION_STATE_H__

struct SAnimation;

#include "SAnimationFrame.h"

struct SAnimationState
{
    SAnimationState( SAnimation* _animation );

    float currentTime;
    float animSpeed;    // 1.0f - normal speed
    bool isLooped;
    bool isPaused;

    SAnimation* animation;

    void AddTime( float time );
    const SAnimationFrame& GetCurrentFrame();
    void Play();
    void Stop();
    void RewindTo( float position ); // 0.0 - poczatek animacji, 1.0 - koniec

};

#endif// __CANIMATION_STATE_H__

