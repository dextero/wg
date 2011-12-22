#include "CNPC.h"
#include "AI/CActorAI.h"
#include "../GUI/CInteractionTooltip.h"
#include "OptionChooser/SignInteraction.h"
#include "CPlayer.h"
#include "../Input/CPlayerController.h"

CNPC::CNPC( const std::wstring& uniqueId )
:	CActor( uniqueId ),
    mInteractionTooltipId(0),
    mInteractionTooltip(NULL)
{
	CActorAI * ai = new CActorAI( this );
	ai->GetData()->SetAttitude( AI_NEUTRAL );
	mController = ai;
}

CNPC::~CNPC()
{
    // obrzydlistwo:
    if (mInteractionTooltip != NULL && mInteractionTooltip->GetId() == mInteractionTooltipId) {
        mInteractionTooltip->Clear();
    }
}

void CNPC::Update(float dt)
{
    mController->Update(dt);
    CActor::Update(dt);
}

void CNPC::HandleCollisionWithPlayer(CPlayer * player) {
    mInteractionTooltip = player->GetController()->GetInteractionTooltip();
    if (mInteractionTooltip->GetHandler() == NULL || mInteractionTooltipId != mInteractionTooltip->GetId()) {
        new SignInteraction(mInteractionTooltip, "Siema!\nJestem Griswold!", player, this);
        mInteractionTooltipId = mInteractionTooltip->GetId();
    }
    mInteractionTooltip->Show();
}
