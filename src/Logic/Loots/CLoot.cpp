#include "CLoot.h"
#include "../../Rendering/ZIndexVals.h"
#include "../CPlayer.h"
#include "../Effects/CEffectHandle.h"
#include "../OptionChooser/CLootItemOptionHandler.h"
//#include "../OptionChooser/CSimpleOptionHandler.h"
#include "../../Input/CPlayerController.h"

CLoot::CLoot(const std::wstring& uniqueId) : 
        CPhysical(uniqueId),
        obj(NULL),
        mOptionHandler(NULL),
        mItem(NULL),
        mLevel(0)
{
    SetZIndex(Z_LOOT);
    SetCategory(PHYSICAL_LOOT);
}

CLoot::~CLoot() {
    if (mOptionHandler) {
        mOptionHandler->Hide();
        mOptionHandler->mReferenceCounter--;
        if (mOptionHandler->mReferenceCounter == 0) {
            delete mOptionHandler;
        }
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
void CLoot::HandleCollision(CPlayer * player)
{
    if (mItem != NULL) {
        if (!mOptionHandler) {
//            mOptionHandler = new CSimpleOptionHandler();
            mOptionHandler = new CLootItemOptionHandler(this);
            mOptionHandler->mReferenceCounter++;
        }
        CInGameOptionChooser * oc = player->GetController()->GetOptionChooser();
        oc->SetOptionHandler(mOptionHandler);
        oc->Show();
        return;
    }

    if (obj->effect) {
        if (mLevel != 0) {
            fprintf(stderr, "loot effect of level %d\n", mLevel);
            EffectSourcePtr source = EffectSourcePtr(NULL);
            ExecutionContextPtr context = ExecutionContextPtr(NULL);
            obj->effect->Apply(player, source, context);
        } else {
            obj->effect->Apply(player);
        }
    }
    MarkForDelete();
}
