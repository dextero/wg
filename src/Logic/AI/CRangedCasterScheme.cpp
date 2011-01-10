#include "CRangedCasterScheme.h"
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

CRangedCasterScheme::CRangedCasterScheme() :
mCombatScheme(NULL),
mMovementScheme(NULL),
mChaserScheme(NULL),
mTurningScheme(NULL)
{
}

CRangedCasterScheme::~CRangedCasterScheme() 
{
}

void CRangedCasterScheme::UpdateAI(CActorAIData *ai, float dt) 
{
	// TODO: keep a distance
	// TODO: start wandering for a random duration if standing and shooting to long
    assert( ai != NULL );
	if (ai->GetMovementScheme() == mMovementScheme){
		if (ai->GetMovementSchemeTimer() > 1.0f){
			//fprintf(stderr,"movement set to NULL\n");
			ai->SetMovementScheme(NULL);
			ai->SetState(esNormal);
		} else {
			mMovementScheme->UpdateAI(ai,dt);
			return;
		}
	}
	
	mCombatScheme->UpdateAI(ai,dt);

	Memory::CSafePtr<CActor> p = ai->GetCombatTarget();
	float len = 1000.0f;
	if (p != NULL){
		len = Maths::Length(p->GetPosition() - ai->GetActor()->GetPosition());
		if (len > 7.0f){
			p = Memory::CSafePtr<CActor>::NullPtr();
		}
	}

	if (p == NULL){
		//fprintf(stderr,"movement set to wanderer\n");
		ai->SetMovementScheme(mMovementScheme);
		mMovementScheme->UpdateAI(ai,dt);
	} else {
		if ((ai->GetMovementScheme() == mMovementScheme) && (ai->GetMovementSchemeTimer() < 1.0f)){
			// nic nie robimy, czekamy az sie poszweda troche
		} else {
			if (len < 3.0f){
				if (ai->GetMovementScheme() != mTurningScheme){
					//fprintf(stderr,"movement set to turning\n");
					ai->SetMovementScheme(mTurningScheme);
				} else {
					if (ai->GetMovementSchemeTimer() > 7.0f){
						//fprintf(stderr,"movement set to wanderer, just so as not to stay in one place to long\n");
						ai->SetMovementScheme(mMovementScheme);
					}
				}
			} else if (ai->GetMovementScheme() != mChaserScheme){				
				ai->SetMovementScheme(mChaserScheme);
				//fprintf(stderr,"movement set to chaser\n");
			}
			if (ai->GetMovementScheme() != NULL){
				ai->GetMovementScheme()->UpdateAI(ai,dt);
			}
		}
	}
}

std::string CRangedCasterScheme::GetName() const
{
    return "CRangedCasterScheme";
}

void CRangedCasterScheme::Init(CAISchemeManager &sm){
	mCombatScheme = sm.GetScheme(L"generic-combat");
	mMovementScheme = sm.GetScheme(L"idle-movement");
	mChaserScheme = sm.GetScheme(L"quickchaser");
	mTurningScheme = sm.GetScheme(L"turning-only");
}
