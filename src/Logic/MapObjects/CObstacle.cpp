#include "CObstacle.h"
#include "../../Rendering/Animations/SAnimation.h"
#include "../../Rendering/ZIndexVals.h"
#include "../../Rendering/Animations/SAnimationState.h"
#include "../../Rendering/CDisplayable.h"

CObstacle::CObstacle(const std::wstring &uniqueId):
    CPhysical(uniqueId),
    mDestroyed(false),
    mDyingTime(0.0f),
    mStats(this),
    mDeathAnim(NULL)
{
    SetZIndex(Z_OBSTACLE);
}

CObstacle::~CObstacle(){
}

CStats *CObstacle::GetStats(){
    return &mStats;
}

void CObstacle::Update( float dt ){
    if (mDestroyed && mDeathAnim){
        mDyingTime += dt;
        if (mDyingTime > mDeathAnim->TotalLength())
            MarkForDelete();
    }
}

void CObstacle::Kill(){
    mDestroyed = true;
    if (mDeathAnim==NULL)
        MarkForDelete();
    else{
        SetAnimation(mDeathAnim);
		GetDisplayable()->GetAnimationState()->isLooped = false;
    }
}

