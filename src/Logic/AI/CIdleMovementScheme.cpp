#include <SFML/System/Vector2.hpp>

#include "CIdleMovementScheme.h"
#include "CActorAI.h"
#include "../CEnemy.h"
#include "../../Map/CMap.h"
#include "../../Map/CMapManager.h"
#include "../../Utils/CRand.h"
#include "CAISchemeManager.h"

CIdleMovementScheme::CIdleMovementScheme() :
mLairGuardian(NULL),
mWanderer(NULL)
{
}

CIdleMovementScheme::~CIdleMovementScheme() 
{
}

void CIdleMovementScheme::UpdateAI(CActorAIData *ai, float dt) 
{
    assert( ai != NULL );
	if ( ai->GetAI() != NULL && ai->GetActor()->GetLair() != NULL) {
		mLairGuardian->UpdateAI(ai,dt);
	} else {
		mWanderer->UpdateAI(ai,dt);
	}
}

std::string CIdleMovementScheme::GetName() const
{
    return "CIdleMovementScheme";
}

void CIdleMovementScheme::Init(CAISchemeManager &sm){
	mLairGuardian = sm.GetScheme(L"lair-guardian");
	mWanderer = sm.GetScheme(L"wanderer");
}


