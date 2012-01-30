#include "CNpc.h"
#include "AI/CActorAI.h"
#include "../GUI/CInteractionTooltip.h"
#include "OptionChooser/ShopInteraction.h"
#include "CPlayer.h"
#include "../Input/CPlayerController.h"

CNpc::CNpc( const std::wstring& uniqueId )
:	CActor( uniqueId ),
    mInteractionTooltipId(0),
    mInteractionTooltip(NULL),
    mSellingItem(""),
    mSellingPrice(0)
{
	CActorAI * ai = new CActorAI( this );
	ai->GetData()->SetAttitude( AI_NEUTRAL );
	mController = ai;
}

CNpc::~CNpc()
{
    // obrzydlistwo:
    if (mInteractionTooltip != NULL && mInteractionTooltip->GetId() == mInteractionTooltipId) {
        mInteractionTooltip->Clear();
    }
}

void CNpc::Update(float dt)
{
    mController->Update(dt);
    CActor::Update(dt);
}

const static int INTERACTION_PRIORITY = 50;

void CNpc::HandleCollisionWithPlayer(CPlayer * player) {
    mInteractionTooltip = player->GetController()->GetInteractionTooltip();
    if (mInteractionTooltip->GetPriority() >= INTERACTION_PRIORITY) {
        return;
    } 
    if (mInteractionTooltip->GetHandler() == NULL || mInteractionTooltipId != mInteractionTooltip->GetId()) {
        new ShopInteraction(mInteractionTooltip, player, this);
        mInteractionTooltipId = mInteractionTooltip->GetId();
        mInteractionTooltip->SetPriority(INTERACTION_PRIORITY);
    }
    mInteractionTooltip->Show();
}

const std::string & CNpc::GetSellingItem() {
    return mSellingItem;
}

void CNpc::SetSellingItem(const std::string & sellingItem) {
    mSellingItem = sellingItem;
}
