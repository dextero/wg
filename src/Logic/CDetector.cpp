#include "CDetector.h"
#include "CPhysical.h"
#include "Effects/CEffectManager.h"
#include "CActor.h"
#include "../Utils/Maths.h"
#include "../Rendering/Animations/CAnimSet.h"
#include "../Utils/CRand.h"
#include "Abilities/CTargetChooser.h"
#include "../Utils/Directions.h"
#include "CPhysicalManager.h"
#include "CNPC.h"
#include "../GUI/Dialogs/CDialogDisplayer.h"

#include <limits>

CDetector::CDetector( const std::wstring& uniqueId ) 
 :	CPhysical( uniqueId ),
	mPhysicalsCategory( 0 ),
	mBehaviour( 0 ),
	mFramesToDeath( -1 ),
    mTimeToDeath(std::numeric_limits<float>::quiet_NaN()),
	mEffectOffset( -1 ),
    mSource(NULL),
    mMeAsSource(NULL),
    mAngularWidth(0.0f),
    mContext(NULL),
	mEnabled( true ),
    mFirstFrame( true ),
	mCollision( false ),
	mOldCollision( false ),
	mNPC( NULL ),
    mCallbackDispatcher( this )
{
	SetCategory( PHYSICAL_DETECTOR );
}

CDetector::~CDetector()
{
    /*if (mBehaviour == SET_COLLISION_FLAG)
        fprintf(stderr,"destroying detector at %p\n",this);*/
    mCallbackDispatcher.Dispatch();
	mCollidingPhysicals.clear();
	mOldCollidingPhysicals.clear();
    if (mMeAsSource != NULL)
        mMeAsSource->Invalidate();
}

void CDetector::SetSource( EffectSourcePtr source ){
    if (mMeAsSource == NULL)
        mMeAsSource = new CEffectSource(this);
    mSource = source;
    mMeAsSource->SetSecondary(mSource);
}

void CDetector::SetExecutionContext( ExecutionContextPtr context ){
    mContext = context;
}

void CDetector::SetCollidingPhysicals(int category)
{
	mPhysicalsCategory = category;
}

void CDetector::SetBehaviour(int behaviour)
{
	mBehaviour = behaviour;
}

void CDetector::SetFramesToDeath(int frames)
{
	mFramesToDeath = frames;
	mEnabled = ( frames != 0 );
}

void CDetector::SetTimeToDeath(float ttd)
{
	mTimeToDeath = ttd;
	mEnabled = ( ttd != 0.0f );
}

void CDetector::SetEffectOffset(int effectOffset)
{
	mEffectOffset = effectOffset;
    //if (mBehaviour & CHOOSE_TARGET_AND_APPLY_EFFECT)
        //fprintf(stderr,"Target choosin' detector at %f,%f\n",GetPosition().x,GetPosition().y);
}

void CDetector::SetMeleeAngle(float angle){
    mAngularWidth = angle;
}

void CDetector::Update(float dt)
{
	if ( (mBehaviour & CHOOSE_NPC_TO_TALK) != 0 && mNPC != NULL )
	{
		fprintf( stderr, "I want to talk to you!\n" );
		gDialogDisplayer.DisplayDialog( ((CNPC*) mNPC)->GetDialogGraph() );
	}

    mFirstFrame = false;

    if ( mFramesToDeath == 0 )
	{
		MarkForDelete();
		mEnabled = false;
		return;
	}

	if ( mFramesToDeath > 0 )
		-- mFramesToDeath;

    /*if ( mTimeToDeath == 0 ){
        if (mTimeToDeath < 0){
		    MarkForDelete();
		    mEnabled = false;
		    return;
        }
        
    }*/

	if ( mTimeToDeath < 0.0f )
	{
		MarkForDelete();
		mEnabled = false;
		return;
	}

	if ( mTimeToDeath > 0.0f )
		mTimeToDeath -= dt;

	mOldCollision = mCollision;
	mCollision = false;
	mNPC = NULL;
	mOldCollidingPhysicals.clear();
	mOldCollidingPhysicals = mCollidingPhysicals; // TODO: poprawic
	mCollidingPhysicals.clear();
	CPhysical::Update( dt );
}

void CDetector::HandleCollision(CPhysical *physical)
{
	if ( mEnabled && (physical->GetCategory() & mPhysicalsCategory) != 0 )
	{
        if ( (mBehaviour & CHECK_MELEE_RANGE ) != 0)
        {
            if (!InMeleeRange(physical))
                return;
        }

		if ( (mBehaviour & SET_COLLISION_FLAG) != 0 )
		{
			mCollision = true;
		}

		if ( (mBehaviour & SAVE_COLLIDING_PHYSICALS) != 0 )
		{
			mCollidingPhysicals.push_back( physical );
		}

		if ( (mBehaviour & APPLY_EFFECT) != 0 && mEffectOffset >= 0 )
		{
            if (mMeAsSource == NULL){
                mMeAsSource = new CEffectSource(this);
                if (mSource != NULL)
                    mMeAsSource->SetSecondary(mSource);
            }
			gEffectManager.ApplyOnce(physical, mEffectOffset,mMeAsSource,mContext );
		}

		if ( (mBehaviour & CHOOSE_NPC_TO_TALK) != 0 && physical->GetCategory() == PHYSICAL_NPC )
		{
			mNPC = physical;
		}
	}
}

bool CDetector::InMeleeRange(CPhysical *victim){
    float d = Maths::LengthSQ(GetPosition() - victim->GetPosition());
    float rq = GetCircleRadius() * GetCircleRadius();
    if (d > rq + victim->GetCircleRadius())
        return false;
    return Maths::CircleInAngle(GetPosition(),RotationToVector((float)GetRotation()),
        mAngularWidth,victim->GetPosition(),victim->GetCircleRadius());
}
