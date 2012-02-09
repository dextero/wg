#include "CNpc.h"
#include "AI/CActorAI.h"
#include "../GUI/CInteractionTooltip.h"
#include "OptionChooser/ShopInteraction.h"
#include "OptionChooser/SignInteraction.h"
#include "CPlayer.h"
#include "../Input/CPlayerController.h"
#include "../GUI/Localization/CLocalizator.h"

// klucze lokalizatora
std::string CNpc::mHints[] = {
    "DLG_NPC_HINT_MAGIC_SCHOOLS",
    "DLG_NPC_HINT_SPELLS",
    "DLG_NPC_HINT_DRAGON",
    "DLG_NPC_HINT_FROZEN_WASTELANDS_MONSTERS",
    "DLG_NPC_HINT_HOSTILE_CREATURES",
    "DLG_NPC_HINT_GARGANT_FOREST",
    "DLG_NPC_HINT_MAP"
};
unsigned int CNpc::mHintsCount = sizeof(CNpc::mHints) / sizeof(CNpc::mHints[0]);


CNpc::CNpc( const std::wstring& uniqueId )
:	CActor( uniqueId ),
    mInteractionTooltipId(0),
    mInteractionTooltip(NULL),
    mSellingItem(""),
    mSellingPrice(0),
    mHasShop(false)
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
        if (mHasShop)
            new ShopInteraction(mInteractionTooltip, player, this);
        else
            new SignInteraction(mInteractionTooltip, StringUtils::ConvertToString(gLocalizator.GetText(mHints[rand() % mHintsCount].c_str())), player, this);

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
    mHasShop = true;
}
