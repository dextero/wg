#include <SFML/System/Vector2.hpp>

#include "CStupidChaserScheme.h"
#include "CActorAI.h"
#include "../CEnemy.h"
#include "../../Utils/CRand.h"
#include "../../Utils/MathsFunc.h"
#include "../CPlayerManager.h"
#include "../CPhysical.h"
#include "../CEnemy.h"
#include "../CActor.h"
#include "../CPlayer.h"

CStupidChaserScheme::CStupidChaserScheme() 
{
}

CStupidChaserScheme::~CStupidChaserScheme() 
{
}

void CStupidChaserScheme::UpdateAI(CActorAIData *ai, float dt) 
{
    assert( ai != NULL );

	/*CPlayer * player0 = gPlayerManager.GetPlayerByNumber(0);
	CPlayer * player1 = gPlayerManager.GetPlayerByNumber(1);

	float player0Distance = ( player0 ? Maths::Length( player0->GetPosition() - ai->GetActor()->GetPosition() ) : 1000.0f );
	float player1Distance = ( player1 ? Maths::Length( player1->GetPosition() - ai->GetActor()->GetPosition() ) : 1000.0f );

	if ( player0Distance < player1Distance )	ai->MoveTo( player0->GetPosition(), player0->GetCircleRadius() );
	else if ( player1Distance < 1000.0f )		ai->MoveTo( player1->GetPosition(), player1->GetCircleRadius() );*/
	Memory::CSafePtr<CActor> tgt = ai->GetCombatTarget();
	if (tgt != NULL){
		ai->MoveTo(tgt->GetPosition(),tgt->GetCircleRadius());
	}
}

std::string CStupidChaserScheme::GetName() const
{
    return "CStupidChaserScheme";
}

void CStupidChaserScheme::Init(CAISchemeManager &sm){
}
