#include "../../Utils/CXml.h"
#include "CLairTemplate.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Rendering/CDisplayable.h"
#include "../CPhysicalManager.h"
#include "CTemplateParam.h"

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CLairTemplate::GetType()
{
	return "CLairTemplate";
}

bool CLairTemplate::Load(CXml &xml)
{
	xml_node<>*	node;

	if ( xml.GetString(0,"type") != "lair" )
		return false;
    CActorTemplate::Load(xml);

	mMinSpawnLimit = xml.GetInt( "spawnLimit", "min" );
	mMaxSpawnLimit = xml.GetInt( "spawnLimit", "max" );

	mMinSpawnRate = xml.GetFloat( "spawnRate", "min" );
	mMaxSpawnRate = xml.GetFloat( "spawnRate", "max" );

	mSpawnPosition.x = xml.GetFloat( "spawnPosition", "x" );
	mSpawnPosition.y = xml.GetFloat( "spawnPosition", "y" );

    mGroupingHint = xml.GetInt("grouping-hint","",1);

    mSpawnSound = xml.GetString( "spawnSound", "filename" );

	for (node=xml.GetChild(0,"monster"); node; node=xml.GetSibl(node,"monster") )
		mMonsters.push_back( xml.GetString(node,"filename") );

	return true;
}

void CLairTemplate::Drop()
{
	delete this;
}

CLair* CLairTemplate::Create(std::wstring id)
{
    CLair* lair = gPhysicalManager.CreateLair(id);

	int spawnLimit =	gRand.Rnd( mMinSpawnLimit,	mMaxSpawnLimit + 1 );
	float spawnRate =	gRand.Rndf( mMinSpawnRate,	mMaxSpawnRate );

	float random =		gRand.Rndf( 0.0f, 1.0f );
	
    Fill(lair,random);
	lair->SetCategory(PHYSICAL_LAIR);
	lair->SetSpawnLimit(spawnLimit);
	lair->SetSpawnRate(spawnRate);
	lair->SetSpawnPosition(mSpawnPosition);
    lair->SetSpawnSound(mSpawnSound);
    lair->SetGroupingHint(mGroupingHint);
    lair->SetGenre(mGenre);

	for ( size_t i = 0; i < mMonsters.size(); i++ )
		lair->AddEnemyType( mMonsters[i] );

    lair->Ready();

	return lair;
}

CTemplateParam *CLairTemplate::ReadParam(CXml &xml, rapidxml::xml_node<> *node, CTemplateParam *p){
    if (p == NULL) p = new CTemplateParam();
    rapidxml::xml_node<> *n = xml.GetChild(node,"monster-count");
    if (n != NULL)
        p->values["monster-count"]=(void*)(xml.GetInt(n,"",-1));
    return CActorTemplate::ReadParam(xml,node,p);
}

void CLairTemplate::Parametrise(CPhysical *phys, CTemplateParam *param){
    CLair *lair = (CLair*)phys;
    if (param->values.count("monster-count") > 0){
        lair->SetMonsterCount((size_t)(param->values["monster-count"]));
    }
    CActorTemplate::Parametrise(lair,param);
}

void CLairTemplate::SerializeParam(std::ostream &out, CTemplateParam *param, int indent){
    if (param != NULL){
        if (param->values.count("monster-count") > 0){
            int monsterCount = (size_t)param->values["monster-count"];
            out << "<monster-count>" << monsterCount << "</monster-count>\n";
        }
    }
    CActorTemplate::SerializeParam(out,param);
}
