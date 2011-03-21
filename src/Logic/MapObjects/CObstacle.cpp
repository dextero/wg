#include "CObstacle.h"
#include "../../Rendering/Animations/SAnimation.h"
#include "../../Rendering/ZIndexVals.h"
#include "../../Rendering/Animations/SAnimationState.h"
#include "../../Rendering/CDisplayable.h"
#include "../CPlayer.h"
#include "../../Input/CPlayerController.h"
#include "../../GUI/CInGameOptionChooser.h"
#include "../OptionChooser/CChestOptionHandler.h"

CObstacle::CObstacle(const std::wstring &uniqueId):
    CPhysical(uniqueId),
    mDestroyed(false),
    mDyingTime(0.0f),
    mStats(this),
    mDeathAnim(NULL),
    mOptionHandler(NULL)
{
    SetZIndex(Z_OBSTACLE);
}

CObstacle::~CObstacle(){
    if (mOptionHandler) {
        mOptionHandler->Hide();
        mOptionHandler->mReferenceCounter--;
        if (mOptionHandler->mReferenceCounter == 0) {
            delete mOptionHandler;
        }
    }
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

void CObstacle::HandleCollisionWithPlayer(CPlayer * player) {
    if (mOptionHandler == NULL) {
        if (mGenre.compare(L"chest")) return;

        mOptionHandler = new CChestOptionHandler(this);
        mOptionHandler->mReferenceCounter++;
    }
    CInGameOptionChooser * oc = player->GetController()->GetOptionChooser();
    oc->SetOptions("open", "close", "explode");
    oc->SetOptionHandler(mOptionHandler);
    oc->Show();
}
