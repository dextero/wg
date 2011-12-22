/*  
 *	Klasa CDetector sluzy do wykrywania kolizji oraz/lub obiektow kolidujacych.
 */

#ifndef _DETECTOR_H_
#define _DETECTOR_H_

#include "CPhysical.h"
#include "CCallbackDispatcher.h"
#include "Effects/CEffectSource.h"
#include "Effects/CExecutionContext.h"
#include <vector>

class CEffectHandle;
class CTargetChooser;

enum detectorBehaviour
{
	SET_COLLISION_FLAG = 1,
	SAVE_COLLIDING_PHYSICALS = 2,
	APPLY_EFFECT = 4,
    CHECK_MELEE_RANGE = 8,
	CHOOSE_NPC_TO_TALK = 16
};

class CDetector : public CPhysical
{
private:
	CDetector( const std::wstring& uniqueId );
	virtual ~CDetector();
	friend class CPhysicalManager;

public:
	void SetCollidingPhysicals( int category );
	void SetBehaviour( int behaviour );
	void SetFramesToDeath( int frames );
    void SetTimeToDeath(float ttd);
	void SetEffectOffset( int effectOffset );
    void SetSource( EffectSourcePtr source );
    void SetExecutionContext( ExecutionContextPtr context );
    void SetMeleeAngle(float angle);

	inline bool GetCollisionFlag()								{ return mOldCollision; }
	inline std::vector<CPhysical*> * GetCollidingPhysicals()	{ return &mOldCollidingPhysicals; }
	inline CCallbackDispatcher *GetCallbackDispatcher()			{ return &mCallbackDispatcher; }

	virtual void Update( float dt );
	void HandleCollision( CPhysical * physical );
private:
	int mPhysicalsCategory;
	int mBehaviour;
	int mFramesToDeath;
	float mTimeToDeath;
	int mEffectOffset;
    EffectSourcePtr mSource;
    EffectSourcePtr mMeAsSource;
    float mAngularWidth;
    ExecutionContextPtr mContext;

	bool mEnabled;
    bool mFirstFrame;
	bool mCollision;
	bool mOldCollision;
	CPhysical* mNpc;
	std::vector<CPhysical*> mCollidingPhysicals;
	std::vector<CPhysical*> mOldCollidingPhysicals;

    CCallbackDispatcher mCallbackDispatcher;
    bool InMeleeRange(CPhysical *victim);
};

#endif

