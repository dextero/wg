#include <SFML/System/Vector2.hpp>

#include "CTurningScheme.h"
#include "CActorAI.h"
#include "../CEnemy.h"
#include "../../Map/CMap.h"
#include "../../Map/CMapManager.h"
#include "../../Utils/CRand.h"

CTurningScheme::CTurningScheme() 
{
}

CTurningScheme::~CTurningScheme() 
{
}

void CTurningScheme::UpdateAI(CActorAIData *ai, float dt) 
{
    assert( ai != NULL );
	
	if (ai->GetCombatTarget() != NULL){
		ai->LookTowards(ai->GetCombatTarget()->GetPosition());
	}
}

std::string CTurningScheme::GetName() const
{
    return "CTurningScheme";
}

void CTurningScheme::Init(CAISchemeManager &sm){
}


