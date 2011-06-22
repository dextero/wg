#include "CEnemyTemplate.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Rendering/CDisplayable.h"
#include "../AI/CAISchemeManager.h"
#include "../AI/CActorAI.h"
#include "../../Rendering/Animations/CAnimSet.h"
#include "../CPhysicalManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../CActor.h"

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CEnemyTemplate::GetType()
{
	return "CEnemyTemplate";
}

bool CEnemyTemplate::Load(CXml &xml)
{
	xml_node<>*	node;

	if ( xml.GetString(0,"type") != "enemy" )
		return false;
    if (!CActorTemplate::Load(xml)) return false;

	for ( node=xml.GetChild(0,"ai"); node; node=xml.GetSibl(node,"ai") )
		mAi.push_back( ConvertToWString( xml.GetString(node,"name") ) );

    if ((node = xml.GetChild(NULL, "bestiary")) != NULL)
    {
        // dane do bestiariusza
        mBestiaryName = xml.GetString(xml.GetChild(node, "name"), "value");
        mBestiaryDesc = xml.GetString(xml.GetChild(node, "desc"), "value");
    }

	return true;
}

void CEnemyTemplate::Drop()
{
	delete this;
}

CEnemy* CEnemyTemplate::Create(std::wstring id)
{
	CEnemy* enemy = gPhysicalManager.CreateEnemy(id);

	float random =	gRand.Rndf( 0.0f, 1.0f );

	enemy->SetCategory(PHYSICAL_MONSTER);
    
    Fill(enemy,random);

	if ( mAi.size() > 0 )
	{
		std::wstring aiScheme = mAi[ gRand.Rnd( 0, static_cast<unsigned>(mAi.size()) ) ];
		enemy->GetAI()->SetScheme( gAISchemeManager.GetScheme(aiScheme) );
	}

	return enemy;
}

