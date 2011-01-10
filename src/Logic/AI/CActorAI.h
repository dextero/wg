#ifndef CACTOR_AI_H_
#define CACTOR_AI_H_

#include <SFML/System/Vector2.hpp>
#include <vector>
#include "../CActorController.h"
#include "../CActor.h"
#include "../../Utils/Memory/CSafePtrable.h"

class CActorFlock;
class CAIScheme;
class IDecorator;

enum enemyState 
{ 
	esNormal, 
	esTurn,
	esWaypoint 
};

enum aiAttitude
{
	AI_ENEMY,
	AI_NEUTRAL,
	AI_ALLY
};

class CActorAI;

class CActorAIData{
private:
	CActorAI *mAI;

	enemyState mState;
	aiAttitude mAttitude;
	
    sf::Vector2f mWaypoint;
    float mWaypointTolerance;
	int mTgtDir;
	Memory::CSafePtr<CActor> mCombatTarget;
	float mMovingTime;

	CAIScheme *mMovementScheme;
	float mMovementSchemeTimer;

	CAIScheme *mCombatScheme;
	float mCombatSchemeTimer;

	CActorFlock *mFlock;

	bool mWaypointSet;
public:
	CActorAIData(CActorAI *AI);
	~CActorAIData(){}

	inline aiAttitude GetAttitude()			{ return mAttitude; }
	inline void SetAttitude(aiAttitude a)	{ mAttitude = a; }

	inline enemyState GetState()			{ return mState; }
    inline void SetState(enemyState State)	{ mState = State; }
	
	inline CActorAI *GetAI()				{ return mAI; }
	CActor *GetActor();

	void Update(float dt);

	inline float GetMovingTime()			{ return mMovingTime; }	
	inline void AddMovingTime(float dt)		{ mMovingTime += dt; }
    void MoveTo(const sf::Vector2f coord, float tolerance = 0.0f);

	inline bool GetWaypointSet()			{ return mWaypointSet; }
	inline void ResetWaypoint()				{ mWaypointSet = false; }
    
	inline sf::Vector2f GetWaypoint()		{ return mWaypoint; }
	inline float GetWaypointTolerance()		{ return mWaypointTolerance; }

    void TurnTowards(int newRot);
	void LookTowards(const sf::Vector2f &pos);

	inline int GetTgtDir(){ return mTgtDir; }
	
	inline void SetMovementScheme(CAIScheme *s){
		if (mMovementScheme != s){
			SetState(esNormal);
			if (mAI != NULL){
				MoveTo(GetActor()->GetPosition(),GetActor()->GetCircleRadius());
			} else {
				MoveTo(GetPosition());
			}
		}
		mMovementScheme = s;
		mMovementSchemeTimer = 0.0f;
	}
	inline CAIScheme *GetMovementScheme(){
		return mMovementScheme;
	}
	inline float GetMovementSchemeTimer(){
		return mMovementSchemeTimer;
	}

	inline void SetCombatScheme(CAIScheme *s){
		mCombatScheme = s;
		mCombatSchemeTimer = 0.0f;
	}
	inline CAIScheme *GetCombatScheme(){
		return mCombatScheme;
	}
	inline float GetCombatSchemeTimer(){
		return mCombatSchemeTimer;
	}	
	
	inline void SetCombatTarget(Memory::CSafePtr<CActor> tgt){
		mCombatTarget = tgt;
	}
	inline Memory::CSafePtr<CActor> GetCombatTarget(){
		return mCombatTarget;
	}

	inline CActorFlock *GetFlock() {
		return mFlock;
	}

	inline void SetFlock(CActorFlock *flock){
		mFlock = flock;
	}

	sf::Vector2f GetPosition();
};

class CActorAI : public CActorController, public Memory::CSafePtrable<CActorAI>{
protected:
	CActorAIData mData;

    float mTurningSharpness;
public:
    CActorAI(CActor *actor);
    virtual ~CActorAI();
    
    inline CAIScheme *GetScheme()			{ return mScheme; }
    void SetScheme(CAIScheme *scheme, float duration = 1.0f);

	inline CActor *GetActor()				{ return mActor; }
    
    void Update( float dt );

    bool ReachedWaypoint();
    
	inline CActorAIData *GetData()			{ return &mData; }
protected:
    CAIScheme *mScheme;

	float mTimeToChangeScheme;

    float CalculateSpeedModifier(float mTurningSharpness);
    void UpdateRotation(float dt);

    std::vector<int> mAvailableAbilities;
    int ChooseAbility();
};

#endif
