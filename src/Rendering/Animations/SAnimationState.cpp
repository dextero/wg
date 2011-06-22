#include "SAnimationState.h"
#include "SAnimation.h"

SAnimationState::SAnimationState( SAnimation* _animation ) :
        currentTime(0),
        animSpeed(1.0f),
        isLooped(true),
        isPaused(false),
        animation( _animation )
{
    ;
} 

void SAnimationState::AddTime( float time )
{
    currentTime += time*animSpeed;

    float animationLength = animation->TotalLength();
    if ( isLooped ) 
    {
        if ( animationLength > 0.0f )
        {
            while ( currentTime > animationLength )
                currentTime -= animationLength; //todo: przyspieszyc
        }
        else
            currentTime = 0.0f;
    }
    else
    {
        if ( currentTime > animationLength )
            currentTime = animationLength;
    }
}

SAnimationFrame gNullFrame = SAnimationFrame( "NULL_ANIMATION" );

const SAnimationFrame& SAnimationState::GetCurrentFrame()
{
    if ( animation->frames.empty() )
        return gNullFrame;

    size_t candidate = 0;
    for ( size_t i = animation->frames.size() - 1 ; i > 0 ; i-- )
    {
        if ( animation->frames[i-1].time < currentTime )
        {
            candidate = i;
            break;
        }
    }
    return animation->frames[ candidate ].frame;
}

void SAnimationState::Play()
{
    isPaused = false;
}

void SAnimationState::Stop()
{
    isPaused = true;
}

void SAnimationState::RewindTo( float position )
{
    if ( position < 0.0f ) position = 0.0f;
    if ( position > 1.0f ) position = 1.0f;
    
   float animationLength = animation->TotalLength();
   currentTime = animationLength * position;
}


