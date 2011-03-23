#include "CLoot.h"
#include "../../Rendering/ZIndexVals.h"
#include "../CActor.h"
#include "../Effects/CEffectHandle.h"
//#include "../../Utils/ToxicUtils.h"
//#include "../../Commands/CCommands.h"

CLoot::CLoot(const std::wstring& uniqueId) : CPhysical(uniqueId), obj(NULL)
{
    SetZIndex(Z_LOOT);
    SetCategory(PHYSICAL_LOOT);
}
//-----------------
void CLoot::Perform(CActor *actor) const
{
    //crimson-mode:
//    if (ToxicUtils::isGameInCrimsonMode && !commandOnTake.empty()) {
//        gCommands.ParseCommand(commandOnTake);
//    }
    if (obj->effect) {
        obj->effect->Apply(actor);
    }
}
