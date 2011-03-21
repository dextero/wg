#include "CNPCTemplate.h"
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

std::string CNPCTemplate::GetType()
{
	return "CNPCTemplate";
}

bool CNPCTemplate::Load(CXml &xml)
{
	xml_node<>*	node;

	if ( xml.GetString(0,"type") != "npc" )
		return false;

    CActorTemplate::Load(xml);

	for ( node=xml.GetChild(0,"dialog"); node; node=xml.GetSibl(node,"dialog") )
		mDialogs.push_back( xml.GetString(node,"file") );
    
	return true;
}

void CNPCTemplate::Drop()
{
	delete this;
}

CNPC* CNPCTemplate::Create(std::wstring id)
{
	CNPC* npc = gPhysicalManager.CreateNPC(id);
	npc->SetCategory(PHYSICAL_NPC);
	Fill(npc,gRand.Rndf());

	for ( unsigned i = 0; i < mDialogs.size(); i++ )
		npc->GetDialogGraph()->AddDialogFile( mDialogs[i] );

	return npc;
}

