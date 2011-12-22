#include "CNpcTemplate.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/Animations/CAnimSet.h"
#include "../CPhysicalManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../CActor.h"

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

	return npc;
}

