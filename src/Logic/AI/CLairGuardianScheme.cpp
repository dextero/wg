#include "CLairGuardianScheme.h"
#include "CActorAI.h"
#include "../CLair.h"
#include "../../Utils/CRand.h"
#include <SFML/System/Vector2.hpp>

CLairGuardianScheme::CLairGuardianScheme() 
{
}

CLairGuardianScheme::~CLairGuardianScheme() 
{
}

void CLairGuardianScheme::UpdateAI(CActorAIData *ai, float dt) 
{
    assert( ai != NULL );

	float maxMovingTime = 2.0f;
	CLair * lair = ai->GetActor()->GetLair();

	if ( ( lair != NULL ) && ( ai->GetMovingTime() > maxMovingTime || ai->GetState() != esWaypoint ) )
	{
	    float range = 3.0f;
		ai->MoveTo( lair->GetPosition() + sf::Vector2f( gRand.Rndf( -range,range ), gRand.Rndf( -range,range ) ) );
	}
}

std::string CLairGuardianScheme::GetName() const
{
    return "CLairGuardianScheme";
}

void CLairGuardianScheme::Init(CAISchemeManager &sm){
}
