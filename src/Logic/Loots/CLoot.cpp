#include "CLoot.h"
#include "../Items/CItem.h"
#include "../../Rendering/ZIndexVals.h"
#include "../CPlayer.h"
#include "../Effects/CEffectHandle.h"
#include "../Effects/CExecutionContext.h"
#include "../../GUI/CInteractionTooltip.h"
#include "../OptionChooser/WeaponInteraction.h"
#include "../../Input/CPlayerController.h"

CLoot::CLoot(const std::wstring& uniqueId) : 
        CPhysical(uniqueId),
        obj(NULL),
        mInteractionTooltipId(0),
        mInteractionTooltip(NULL),
        mItem(NULL),
        mLevel(0)
{
    SetZIndex(Z_LOOT);
    SetCategory(PHYSICAL_LOOT);
}

CLoot::~CLoot() {
    // obrzydlistwo:
    if (mInteractionTooltip != NULL && mInteractionTooltip->GetId() == mInteractionTooltipId) {
        mInteractionTooltip->Clear();
    }    
}

//weapon:
void CLoot::SetAbility(const std::string & ability)
{
    if (ability.empty()) return;

    CItem * item = new CItem();
    item->SetAbility(ability);
    mItem = item;
}

//-----------------
const static int INTERACTION_PRIORITY = 100;
void CLoot::HandleCollision(CPlayer * player)
{
    if (mItem != NULL) {
        mInteractionTooltip = player->GetController()->GetInteractionTooltip();
        if (mInteractionTooltip->GetPriority() >= INTERACTION_PRIORITY) {
            return;
        }
        if (mInteractionTooltip->GetHandler() == NULL || mInteractionTooltipId != mInteractionTooltip->GetId()) {
            new WeaponInteraction(mInteractionTooltip, player, this);
            mInteractionTooltipId = mInteractionTooltip->GetId();
            mInteractionTooltip->SetPriority(INTERACTION_PRIORITY);
        }
        mInteractionTooltip->Show();
        return;
    }

    if (obj->effect) {
        if (mLevel != 0) {
            EffectSourcePtr source = EffectSourcePtr(NULL);
            ExecutionContextPtr context = CExecutionContext::Alloc(player->GetSafePointer(),player,mLevel,player->GetPinnedAbilityBatch());    //CHECK
            obj->effect->Apply(player, source, context);
        } else {
            obj->effect->Apply(player);
        }
    }
    MarkForDelete();
}
