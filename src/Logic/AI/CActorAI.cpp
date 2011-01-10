#include "CActorAI.h"

#include "../CActor.h"
#include "CAIScheme.h"
#include "CAISchemeManager.h"
#include "CAIAbilityHint.h"

#include <cmath>
#include <cstdio>

#include "../CPlayer.h"
#include "../CPlayerManager.h"
#include "../Abilities/CAbility.h"
#include "../../Audio/CAudioManager.h"

#include "../../Utils/MathsFunc.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Directions.h"
#include "../../Utils/HRTimer.h"

#include "CActorFlock.h"

CActorAIData::CActorAIData(CActorAI *AI):
	mAI(AI),
	mState(esNormal),
	mAttitude(AI_ENEMY),
    mWaypoint(sf::Vector2f(0.0f,0.0f)),
	mWaypointTolerance(0.0f),
    mTgtDir(0),
	mCombatTarget(Memory::CSafePtr<CActor>::NullPtr()),
	mMovingTime(0.0f),
	mMovementScheme(NULL),
	mMovementSchemeTimer(0.0f),
	mCombatScheme(NULL),
	mCombatSchemeTimer(0.0f),
	mFlock(NULL),
	mWaypointSet(false)
{
}

CActor *CActorAIData::GetActor(){
	return mAI->GetActor();
}

void CActorAIData::MoveTo(const sf::Vector2f coord, float tolerance)
{
    mWaypoint = coord;
    mWaypointTolerance = tolerance;
	mMovingTime = 0.0f;
    mState = esWaypoint;
	mWaypointSet = true;
	if (Maths::Length(coord - sf::Vector2f(0.0f,0.0f)) < 1.0f){
		mWaypointSet = true;
	}
}

void CActorAIData::LookTowards(const sf::Vector2f &pos){
	TurnTowards(RotationFromVector(pos - GetActor()->GetPosition()));
	if (mState != esWaypoint)
		mState = esTurn;
}

void CActorAIData::TurnTowards(int newRot)
{
    mTgtDir = newRot;
}

void CActorAIData::Update(float dt){
	mCombatSchemeTimer+=dt;
	mMovementSchemeTimer+=dt;
}

sf::Vector2f CActorAIData::GetPosition(){
	if (mAI != NULL){
		return GetActor()->GetPosition();
	} else {
		return mFlock->GetCenter();
	}
}


CActorAI::CActorAI(CActor *actor) :
    CActorController(actor),
	mData(this),
	mTurningSharpness(0.0f),
	mScheme(NULL),
	mTimeToChangeScheme(0.0f)
{

}

CActorAI::~CActorAI()
{}

void CActorAI::SetScheme(CAIScheme *scheme, float duration)
{
    if (mScheme)
		mScheme->UnregisterAI(this->GetSafePointer());
    
	mScheme = scheme;
	mScheme->RegisterAI(this->GetSafePointer());
    mTimeToChangeScheme = duration;
}

void CActorAI::UpdateRotation(float dt)
{
    float rot = float( GetActor()->GetRotation() );
	mTurningSharpness = Maths::Abs( rot - mData.GetTgtDir() );

    if ( mTurningSharpness < 1.0f )
	{
        mTurningSharpness = 0.0f;
        return;
	}

	float maxTurn = GetActor()->GetMaxTurnSpeed() * dt;
	int tgtDir = mData.GetTgtDir();
    float angleByLeft = tgtDir - rot; 
	if ( angleByLeft < 0.0f ) angleByLeft = std::fmod(angleByLeft, 360.0f) + 360.0f;
    float angleByRight = rot - tgtDir; 
	if ( angleByRight < 0.0f ) angleByRight = std::fmod(angleByRight, 360.0f) + 360.0f;

    if ( angleByLeft < angleByRight)	rot = std::min( angleByLeft, maxTurn );			// w lewo
	else								rot = std::min( angleByRight, maxTurn ) * -1.0f;// w prawo

    TurnBy(rot);
}

float CActorAI::CalculateSpeedModifier(float mTurningSharpness)
{
    return 0.6f + 0.2f * (1.0f + cosf(mTurningSharpness*3.14159265f/180.0f));
}

void CActorAI::Update(float dt)
{
    CActorController::Update(dt);

	mData.Update(dt);

    if ( GetActor()->GetSpawnState() != CActor::ssAlive )	return;
	if ( mScheme )											mScheme->UpdateAI(&mData, dt);

	// TODO: Dodac rozne zachowania w zaleznosci od nastawienia do gracza
	// TEMP:
	if ( mData.GetAttitude() != AI_ENEMY )
		return;

	// wykonaj ruch / obrot
    if ( mData.GetState() == esWaypoint )
	{
		mData.AddMovingTime(dt);

		sf::Vector2f v = mData.GetWaypoint() - GetActor()->GetPosition();
        float len = Maths::Length( v );
		float tolerance = GetActor()->GetCircleRadius() + mData.GetWaypointTolerance();

        mData.TurnTowards( RotationFromVector( Maths::Normalize(v) ) );
        UpdateRotation( dt );
        if ( len < GetActor()->GetStats()->GetCurrAspect(aSpeed) * dt + tolerance )
		{
			mData.SetState(esNormal);
			GetActor()->SetVelocity( Maths::VectorZero() );
			if ( len > tolerance )
				GetActor()->SetPosition( GetActor()->GetPosition() + Maths::Normalize( v ) * ( len - tolerance ) );
		}
		else
		{
			float speedModifier = CalculateSpeedModifier(mTurningSharpness);

			SetMove(Maths::Sgn(speedModifier));
            SetSpeed(GetActor()->GetStats()->GetCurrAspect(aSpeed)*(speedModifier>0.0f?speedModifier:-speedModifier));
        }
    }
	else if ( mData.GetState() == esTurn ){
		UpdateRotation( dt );
		
		float rot = float( GetActor()->GetRotation() );
		if (Maths::Abs( rot - mData.GetTgtDir() ) < 0.0001f)
			mData.SetState(esNormal);
	} else
		SetSpeed( GetActor()->GetStats()->GetCurrAspect(aSpeed) );

	// atak
	for ( unsigned i = 0; i < gPlayerManager.GetPlayerCount(); i++ )
	{
		CPlayer *player = gPlayerManager.GetPlayerByOrder( i );
        if (player == NULL) continue;
        mAvailableAbilities.clear();
        GetActor()->GetAbilityPerformer().GetAvailableAbilities(mAvailableAbilities);
        if (mAvailableAbilities.size() > 0){
            int idx = ChooseAbility(); /*GetActor()->GetAbilityPerformer().GetDefaultAttack() */
		    CAbility *abi = GetActor()->GetAbilityPerformer().GetAbility( idx )->ability;
			ExecutionContextPtr context = GetActor()->GetAbilityPerformer().GetContext(idx);
            if (abi == NULL) continue;
            bool useAbility = false;
            if (abi->mAIHint == NULL)
		        useAbility = abi->InMeleeRange( GetActor(), player, context );
            else
                useAbility = abi->mAIHint->UseAbility( GetActor(), player);
            if (useAbility)
			{
			    GetActor()->GetAbilityPerformer().PerformAbility( idx );

				//odegraj dzwiek
			    if ( unsigned sndCount = GetActor()->GetAttackSounds().size() )
				    gAudioManager.GetSoundPlayer().Play( GetActor()->GetAttackSounds()[ rand() % sndCount ], GetActor()->GetPosition() );
  			} 
        }
	}
}

int CActorAI::ChooseAbility(){
    int r = gRand.Rnd(0,mAvailableAbilities.size());
    if ((r >= 0) && ((unsigned int)r < mAvailableAbilities.size()))
        return mAvailableAbilities[r];
    else
        return -1;
}

bool CActorAI::ReachedWaypoint()
{
	sf::Vector2f v = mData.GetWaypoint() - GetActor()->GetPosition();
    float len = Maths::Length( v );
    float tolerance = GetActor()->GetCircleRadius() + mData.GetWaypointTolerance();

    return (len < tolerance);
}