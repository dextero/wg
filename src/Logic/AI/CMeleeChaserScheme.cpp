#include "CMeleeChaserScheme.h"
#include "CActorAI.h"
#include "../CPlayerManager.h"
#include "../CPlayer.h"
#include "../CLair.h"
#include "../CEnemy.h"
#include "../../Utils/CRand.h"
#include "CActorAI.h"
#include "../../Utils/MathsFunc.h"
#include "CAISchemeManager.h"
#include <SFML/System/Vector2.hpp>

CMeleeChaserScheme::CMeleeChaserScheme():
    mCombatScheme(NULL),
    mMovementScheme(NULL),
    mChaserScheme(NULL)
{
}

CMeleeChaserScheme::~CMeleeChaserScheme() 
{
}

void CMeleeChaserScheme::RegisterAI(Memory::CSafePtr<CActorAI> ai)
{
    mCombatScheme->RegisterAI(ai);
    mMovementScheme->RegisterAI(ai);
    mChaserScheme->RegisterAI(ai);
}

void CMeleeChaserScheme::UnregisterAI(Memory::CSafePtr<CActorAI> ai)
{
    mCombatScheme->UnregisterAI(ai);
    mMovementScheme->UnregisterAI(ai);
    mChaserScheme->UnregisterAI(ai);
}

void CMeleeChaserScheme::UpdateAI(CActorAIData *ai, float dt) 
{
	assert( ai != NULL );
	if (ai->GetMovementScheme() == mMovementScheme){
		if (ai->GetMovementSchemeTimer() > 1.0f){
			ai->SetMovementScheme(NULL);
		} else {
			mMovementScheme->UpdateAI(ai,dt);
			return;
		}
	}
	
	mCombatScheme->UpdateAI(ai,dt);

	Memory::CSafePtr<CActor> p = ai->GetCombatTarget();
	if (p == NULL){
		ai->SetMovementScheme(mMovementScheme);
		mMovementScheme->UpdateAI(ai,dt);
	} else {
		ai->SetMovementScheme(mChaserScheme);
		mChaserScheme->UpdateAI(ai,dt);
	}
}

std::string CMeleeChaserScheme::GetName() const
{
    return "CMeleeChaserScheme";
}

void CMeleeChaserScheme::Init(CAISchemeManager &sm){
	mCombatScheme = sm.GetScheme(L"generic-combat");
	mMovementScheme = sm.GetScheme(L"idle-movement");
	mChaserScheme = sm.GetScheme(L"quickchaser");
}
