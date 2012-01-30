#include "CObstacle.h"
#include "../../Rendering/Animations/SAnimation.h"
#include "../../Rendering/ZIndexVals.h"
#include "../../Rendering/Animations/SAnimationState.h"
#include "../../Rendering/CDisplayable.h"
#include "../CPlayer.h"
#include "../../Input/CPlayerController.h"
#include "../../GUI/CInGameOptionChooser.h"
#include "../../GUI/CInteractionTooltip.h"
#include "../OptionChooser/SignInteraction.h"
#include "../OptionChooser/ChestInteraction.h"

CObstacle::CObstacle(const std::wstring &uniqueId):
    CPhysical(uniqueId),
    mDestroyed(false),
    mDyingTime(0.0f),
    mStats(NULL),
    mTitle(""),
    mDeathAnim(NULL),
    mInteractionTooltipId(0),
    mInteractionTooltip(NULL)
{
    mStats = this;
    SetZIndex(Z_OBSTACLE);
}

CObstacle::~CObstacle(){
    if (mInteractionTooltip != NULL && mInteractionTooltip->GetId() == mInteractionTooltipId) {
        mInteractionTooltip->Clear();
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

const static int CHEST_INTERACTION_PRIORITY = 75;

void CObstacle::HandleCollisionWithPlayer(CPlayer * player) {
    if (!mTitle.empty()) {
        mInteractionTooltip = player->GetController()->GetInteractionTooltip();
        if (mInteractionTooltip->GetHandler() == NULL || mInteractionTooltipId != mInteractionTooltip->GetId()) {
            new SignInteraction(mInteractionTooltip, mTitle, player, this);
            mInteractionTooltipId = mInteractionTooltip->GetId();
        } 
        mInteractionTooltip->Show();
        return;
    }

    if (mGenre == L"chest") {
        mInteractionTooltip = player->GetController()->GetInteractionTooltip();
        if (mInteractionTooltip->GetPriority() >= CHEST_INTERACTION_PRIORITY) {
            return;
        }
        if (mInteractionTooltip->GetHandler() == NULL || mInteractionTooltipId != mInteractionTooltip->GetId()) {
            new ChestInteraction(mInteractionTooltip, player, this);
            mInteractionTooltipId = mInteractionTooltip->GetId();
            mInteractionTooltip->SetPriority(CHEST_INTERACTION_PRIORITY);
        }
        mInteractionTooltip->Show();
    }
}

void CObstacle::SetTitle(const std::string & title) {
    mTitle = title;
}

