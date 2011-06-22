#include <SFML/System/Vector2.hpp>

#include "CWandererScheme.h"
#include "CActorAI.h"
#include "../CEnemy.h"
#include "../../Map/CMap.h"
#include "../../Map/CMapManager.h"
#include "../../Utils/CRand.h"

CWandererScheme::CWandererScheme() 
{
}

CWandererScheme::~CWandererScheme() 
{
}

void CWandererScheme::UpdateAI(CActorAIData *ai, float dt) 
{
    assert( ai != NULL );
	float maxMovingTime = 2.0f;

	if ( ai != NULL && ( ai->GetMovingTime() > maxMovingTime || ai->GetState() != esWaypoint ) ) 
	{
        Map::CMap *map = gMapManager.GetCurrent();
        if (map != NULL){
            float r = ai->GetActor()->GetCircleRadius();
            float w = (float) map->GetSize().x - r;
            float h = (float) map->GetSize().y - r;
            ai->MoveTo( sf::Vector2f( gRand.Rndf(r,w), gRand.Rndf(r,h) ) );
        }
    }
}

std::string CWandererScheme::GetName() const
{
    return "CWandererScheme";
}

void CWandererScheme::Init(CAISchemeManager &sm){
}


