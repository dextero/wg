#include "CQuickChaserScheme.h"
#include "CActorAI.h"
#include "../CPlayerManager.h"
#include "../CPlayer.h"
#include "../CEnemy.h"
#include "../CLair.h"
#include "../../Utils/CRand.h"
#include "../../Utils/MathsFunc.h"
#include "CActorFlock.h"
#include "CAISchemeManager.h"
#include <SFML/System/Vector2.hpp>
#include <iostream>

CShortestPaths * CQuickChaserScheme::mNarrowPathsToPlayer0 = NULL;
CShortestPaths * CQuickChaserScheme::mWidePathsToPlayer0 = NULL;
CShortestPaths * CQuickChaserScheme::mNarrowPathsToPlayer1 = NULL;
CShortestPaths * CQuickChaserScheme::mWidePathsToPlayer1 = NULL;

CQuickChaserScheme::CQuickChaserScheme():
mStupidChaserScheme(NULL),
mIdleChaserScheme(NULL)
{
	if ( mNarrowPathsToPlayer0 == NULL )
	{
		mNarrowPathsToPlayer0 = new CShortestPaths;
		mNarrowPathsToPlayer0->Initialize( L"player0", 10.0f, .99f );
		mNarrowPathsToPlayer0->Update();
	}

	if ( mWidePathsToPlayer0 == NULL )
	{
		mWidePathsToPlayer0 = new CShortestPaths;
		mWidePathsToPlayer0->Initialize( L"player0", 10.0f, 1.99f );
		mWidePathsToPlayer0->Update();
	}

	if ( mNarrowPathsToPlayer1 == NULL )
	{
		mNarrowPathsToPlayer1 = new CShortestPaths;
		mNarrowPathsToPlayer1->Initialize( L"player1", 10.0f, .99f );
		mNarrowPathsToPlayer1->Update();
	}

	if ( mWidePathsToPlayer1 == NULL )
	{
		mWidePathsToPlayer1 = new CShortestPaths;
		mWidePathsToPlayer1->Initialize( L"player1", 10.0f, 1.99f );
		mWidePathsToPlayer1->Update();
	}
}

CQuickChaserScheme::~CQuickChaserScheme() 
{
}

void CQuickChaserScheme::UpdateAI(CActorAIData *ai, float dt) 
{
    assert( ai != NULL );

	bool validWayPoint = false;
	sf::Vector2f wayPoint;

	/*float distToPlayer0 = 100000.0f;
	float distToPlayer1 = 100000.0f;

	if ( CPlayer * p = gPlayerManager.GetPlayerByNumber(0) )	distToPlayer0 = Maths::Length( p->GetPosition() - ai->GetActor()->GetPosition() );
	if ( CPlayer * p = gPlayerManager.GetPlayerByNumber(1) )	distToPlayer1 = Maths::Length( p->GetPosition() - ai->GetActor()->GetPosition() );*/

	/* pobierz nastepny waypoint do najblizszego gracza */

	CShortestPaths *myPaths = NULL;
	Memory::CSafePtr<CActor> tgt = ai->GetCombatTarget();
	if (tgt != NULL){
		// jesli jestesmy blisko celu, stupid wystarczy
		if (Maths::Length( tgt->GetPosition() - ai->GetPosition() ) < 3.0f){
			mStupidChaserScheme->UpdateAI(ai,dt);
			return;
		}
	}
	CPlayer * p0 = gPlayerManager.GetPlayerByNumber(0);
	CPlayer * p1 = gPlayerManager.GetPlayerByNumber(1);
	bool isWide = false; 
	if (ai->GetAI() != NULL)
		isWide = ai->GetActor()->GetCircleRadius() >= 0.5f;
	if (tgt == p0){
		myPaths = isWide ? mWidePathsToPlayer0 : mNarrowPathsToPlayer0;
	} else if (tgt == p1){
		myPaths = isWide ? mWidePathsToPlayer1 : mNarrowPathsToPlayer1;
	}

	/*if ( distToPlayer0 < distToPlayer1 )
	{
		if ( ai->GetActor()->GetCircleRadius() < 0.5f )	validWayPoint = mNarrowPathsToPlayer0->GetNextWayPoint( ai->GetActor(), &wayPoint );
		else											validWayPoint = mWidePathsToPlayer0->GetNextWayPoint( ai->GetActor(), &wayPoint );
	}
	else
	{
		if ( ai->GetActor()->GetCircleRadius() < 0.5f )	validWayPoint = mNarrowPathsToPlayer1->GetNextWayPoint( ai->GetActor(), &wayPoint );
		else											validWayPoint = mWidePathsToPlayer1->GetNextWayPoint( ai->GetActor(), &wayPoint );
	}*/
	//bool ignoreWalls = ai->GetAI() == NULL;
	if (myPaths !=NULL){
		validWayPoint =  myPaths->GetNextWayPoint(ai,&wayPoint);
	}

	/* jesli waypoint jest prawidlowy... */

	if ( validWayPoint )	
	{
		if (ai->GetAI() != NULL)
			ai->MoveTo( wayPoint, -ai->GetActor()->GetCircleRadius() );
		else
			ai->MoveTo( wayPoint, ai->GetFlock()->GetAvgDistance() );
	}

	/* jesli nie - wygeneruj losowy */

	/*else
	{
		static float timeToChangeRandomWayPoint = 0.0f;
		if ( timeToChangeRandomWayPoint < 0.0f )	
		{
			ai->MoveTo( ai->GetActor()->GetPosition() + sf::Vector2f( gRand.Rndf( -10.0f, 10.0f ), gRand.Rndf( -10.0f, 10.0f ) ) );
			timeToChangeRandomWayPoint = 2.0f;
		}
		else
		{
			timeToChangeRandomWayPoint -= dt;
		}
	}*/

	/* jesli nie - default to idle movement chaser */
	else 
	{
		mIdleChaserScheme->UpdateAI(ai,dt);
	}
}

std::string CQuickChaserScheme::GetName() const
{
    return "CQuickChaserScheme";
}

void CQuickChaserScheme::Init(CAISchemeManager &sm){
	mIdleChaserScheme = sm.GetScheme(L"idle-movement");
	mStupidChaserScheme = sm.GetScheme(L"stupidchaser");
}
