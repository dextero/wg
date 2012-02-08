#include "CNpcTemplate.h"
#include "CTemplateParam.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/Animations/CAnimSet.h"
#include "../CPhysicalManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../CActor.h"
#include "../CNpc.h"
#include "../../Map/CRandomMapGenerator.h"
#include "../CPlayerManager.h"
#include "../CPlayer.h"

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CNpcTemplate::GetType()
{
	return "CNpcTemplate";
}

bool CNpcTemplate::Load(CXml &xml)
{
	xml_node<>*	node;

	if ( xml.GetString(0,"type") != "npc" )
		return false;

    CActorTemplate::Load(xml);

	for ( node=xml.GetChild(0,"dialog"); node; node=xml.GetSibl(node,"dialog") )
		mDialogs.push_back( xml.GetString(node,"file") );
    
	return true;
}

void CNpcTemplate::Drop()
{
	delete this;
}

CNpc* CNpcTemplate::Create(std::wstring id)
{
	CNpc* npc = gPhysicalManager.CreateNpc(id);
	npc->SetCategory(PHYSICAL_NPC);
	Fill(npc,gRand.Rndf());

	for ( unsigned i = 0; i < mDialogs.size(); i++ )
		npc->GetDialogGraph()->AddDialogFile( mDialogs[i] );

    int level = 0;
    for (unsigned int i = 0; i < gPlayerManager.GetPlayerCount(); ++i)
        level += gPlayerManager.GetPlayerByNumber(0)->GetLevel();
    level /= gPlayerManager.GetPlayerCount();

    npc->SetSellingItem(gRandomMapGenerator.GetRandomWeaponFile(level));

	return npc;
}

CTemplateParam * CNpcTemplate::ReadParam(CXml & xml, rapidxml::xml_node<> * node, CTemplateParam * orig) {
    if (orig == NULL) orig = new CTemplateParam();
    std::string sellingItemStr = xml.GetString(node, "sellingItem");
    if (!sellingItemStr.empty())
        orig->stringValues["sellingItem"] = sellingItemStr;
    return CActorTemplate::ReadParam(xml,node,orig);
}

void CNpcTemplate::Parametrise(CPhysical * phys, CTemplateParam * param) {
    CNpc * npc = (CNpc*)phys;
    if (param->stringValues.count("sellingItem") > 0) {
        std::vector<std::string> sellingItemStr = StringUtils::Explode(param->stringValues["sellingItem"], ":");
        npc->SetSellingItem(sellingItemStr[0]);
        npc->SetSellingPrice(StringUtils::Parse<int>(sellingItemStr[1]));
    }
    CActorTemplate::Parametrise(npc, param);
}

void CNpcTemplate::SerializeParam(std::ostream &out, CTemplateParam *param, int indent)
{
    //todo:!!!
    CActorTemplate::SerializeParam(out, param);
}

