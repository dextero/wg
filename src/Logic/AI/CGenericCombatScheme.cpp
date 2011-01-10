#include "CGenericCombatScheme.h"
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

CGenericCombatScheme::CGenericCombatScheme() 
{
}

CGenericCombatScheme::~CGenericCombatScheme() 
{
}

void CGenericCombatScheme::UpdateAI(CActorAIData *ai, float dt) 
{
	// TODO: wybor najlepszej umiejki/czaru
	// TODO: utrzymywanie tego samego celu, jesli sie da
	// TODO: atakowanie gracza, ktory mnie zaatakowal, jesli aktualny cel mnie od dawna nie ranil
	// TODO: zmiana celu, jesli aktualny gracz jest trudno dostepny (??)
    assert( ai != NULL );
	unsigned int cnt = gPlayerManager.GetPlayerCount();
	std::vector<CPlayer*> players;
	players.resize(cnt);
	for(unsigned int i = 0; i < cnt; i++) players[i] = gPlayerManager.GetPlayerByNumber(i);
	
	std::vector<float> distance; 
	distance.resize(cnt);
	for(unsigned int i = 0; i < cnt; i++)
		distance[i] = ( players[i] ? Maths::Length( players[i]->GetPosition() - ai->GetPosition() ) : 1000.0f );

	float bestDist = 1000.0f;
	int bestPlayer = -1;
	for (unsigned int i = 0; i < cnt; i++){
		if (bestDist > distance[i]){
			bestPlayer = i;
			bestDist = distance[i];
		}
	}
	if (bestPlayer == -1){
		ai->SetCombatTarget(Memory::CSafePtr<CActor>::NullPtr());
	} else {
		ai->SetCombatTarget(players[bestPlayer]->GetSafePointer());
	}
}

std::string CGenericCombatScheme::GetName() const
{
    return "CGenericCombatScheme";
}

void CGenericCombatScheme::Init(CAISchemeManager &sm){
}
