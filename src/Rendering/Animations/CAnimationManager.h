#ifndef __CANIMATION_MANAGER_H__
#define __CANIMATION_MANAGER_H__

#include "../../IFrameListener.h"
#include "../../Utils/CSingleton.h"

#include <string>
#include <vector>
#include <map>
#include <set>

struct SAnimation;
struct SAnimationState;

#define gAnimationManager CAnimationManager::GetSingleton()

class CAnimationManager: public CSingleton<CAnimationManager> , IFrameListener
{
public:
    CAnimationManager();
    ~CAnimationManager();

#ifdef __EDITOR__
    // w edytorze wlaczamy animacje, a co :)
    virtual bool FramesDuringPause(EPauseVariant pv) { return true; }
#endif

    virtual void FrameStarted(float secondsPassed);

    SAnimation* GetAnimation( const std::string& animationName );
    SAnimationState* CreateAnimationState( const std::string& animationName );
    SAnimationState* CreateAnimationState( SAnimation *animationPtr );
    void DestroyAnimationState( SAnimationState* animationState );

private:
    // ponizsze przewalic do PIMPL (yikes!)
    // narazie niech tu sobie polezy roboczo:

    // TODO: wstring?
    std::map< std::string, SAnimation* > mAnimations;

    std::set< SAnimationState* > mAnimationStates;
};

#endif /*__CANIMATION_MANAGER_H__*/
