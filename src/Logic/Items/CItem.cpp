#include "CItem.h"
#include "../Abilities/CAbility.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../Utils/CXml.h"
#include "../../ResourceManager/CImage.h"
#include "../../Rendering/ZIndexVals.h"
#include "../CActor.h"
#include "../../Utils/ToxicUtils.h"
#include "../../Commands/CCommands.h"

CItem::CItem(const std::wstring& uniqueId) : CPhysical(uniqueId), obj(NULL)
{
    SetZIndex(Z_ITEM);
    SetCategory(PHYSICAL_ITEM);
}
//-----------------
void CItem::Perform(CActor *actor) const
{
    //crimson-mode:
    if (ToxicUtils::isGameInCrimsonMode && !commandOnTake.empty()) {
        gCommands.ParseCommand(commandOnTake);
    }
    if (obj->effect) {
        obj->effect->Apply(actor);
    }
}
