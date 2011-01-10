#include "CActorController.h"
#include "CActor.h"
#include "../Utils/Directions.h"
#include "../Utils/MathsFunc.h"
#include "CPlayerManager.h"
#include <cmath>

CActorController::CActorController(CActor *actor):
    mStrafe(0),
    mMove(0)
{
    mActor = actor;
    mTrueRot = 0.0f;
}


CActorController::~CActorController(){
}

void CActorController::SetSpeed(float value){
    if (mActor->GetSpawnState() == CActor::ssAlive)
	{
		// Uwzglêdniam strafe
		sf::Vector2f dir = RotationToVector((float)mActor->GetRotation());
		sf::Vector2f move;

        if (mStrafe !=0) {
			move += Maths::VectorRightOf(dir) * (float) mStrafe;
            mActor->mMoveState = CActor::msStrafing;
        }

        if(mMove !=0) {
			move += dir * (float) mMove;
            mActor->mMoveState = CActor::msWalking;
        }

        if ((mMove == 0) && (mStrafe == 0))
            return;

		move = Maths::Normalize(move);

        mActor->SetVelocity(move*value);
    }
}

void CActorController::TurnBy(float value){
    if (mActor->GetSpawnState() == CActor::ssAlive){
        mTrueRot+=value;
        while (mTrueRot < 0.0f) mTrueRot += 360.0f;
        while (mTrueRot > 360.0f) mTrueRot -= 360.0f;
        mActor->SetRotation((int)mTrueRot);
        if ((value >= 0.001f) || (value <= -0.001))
            mActor->mMoveState = CActor::msTurning;
    }
}

void CActorController::Update(float dt){
    if (mActor->GetSpawnState() != CActor::ssAlive){
        mActor->SetVelocity(sf::Vector2f(0.0f,0.0f));
    }
    mActor->mMoveState = CActor::msStanding;
	mMove = 0;
	mStrafe = 0;
}

float CActorController::GetTrueRot()
{
	return mTrueRot;
}

void CActorController::SetMove(int value)
{
	mMove = value;
}

void CActorController::SetStrafe(int value)
{
	mStrafe = value;
}


void CActorController::GotDamageFrom(float dmg, EffectSourcePtr source){
    if (source != EffectSourcePtr(NULL)){
        CPhysical *dealer = source->DeterminePhysicalSource();
        if (dealer){
            /*if (gPlayerManager.VerifyKnownPhysical(dealer))
                fprintf(stderr,"Physical %ls got damage from %ls\n",mActor->GetUniqueId().c_str(), dealer->GetUniqueId().c_str());
            else
                fprintf(stderr,"Physical %ls got damage from source that is unverified!\n",mActor->GetUniqueId().c_str());*/
        } else {
            //fprintf(stderr,"Physical %ls got damage from source that couldnt find its dealer!\n",mActor->GetUniqueId().c_str());
            //source->PrintToStderr();
        }
    } else {
        //fprintf(stderr,"Physical %ls got damage from unknown source!\n",mActor->GetUniqueId().c_str());
    }
}
