#include <limits>
#include <cmath>

#include "CBullet.h"
#include "../Utils/MathsFunc.h"
#include "../Utils/Directions.h"
#include "CActor.h"
#include "Effects/CEffectHandle.h"
#include "CDetector.h"
#include "CPhysicalManager.h"
#include "Abilities/CTargetChooser.h"

CBullet::CBullet(const std::wstring &uniqueId) :
    CPhysical( uniqueId ),
    mAppliedEffects(this),
    TTL(std::numeric_limits<float>::infinity()),
    range(std::numeric_limits<float>::infinity()),
    source(NULL),
    meAsSource(NULL),
    rotSpeed(0),
    acceleration(0.0f),
    explodeOnFade(0),
    filter(0),
	mTimeForTracking(-1.0f),
	mTurningSpeed(0.0f),
	mTarget(Memory::CSafePtr<CActor>::NullPtr()),
	mTargetChooser(NULL),
    rotLeftover(0.0f),
    mEffect(0),
    mExpl(0)
{

}

void CBullet::Explode(){
    if (GetExplosionEffect()!=NULL) {
        EffectSourcePtr src;
        if (this->source == EffectSourcePtr(NULL)){
            if (meAsSource == NULL)
                meAsSource = new CEffectSource(this);
            src = meAsSource;
        } else{
            src = new CEffectSource(this);
            src->SetSecondary(source);
        }
        GetExplosionEffect()->PerformAt(GetPosition(),src,context);
    }
}

void CBullet::Update(float dt){
    CPhysical::Update(dt);
    mAppliedEffects.Update(dt);
    TTL-=dt;
    if (TTL < 0.0f){
        this->MarkForDelete();
        if (explodeOnFade)
            Explode();
    }
    range -= Maths::Length(mOldPosition-mPosition);
    if (range < 0.0f){
        this->MarkForDelete();
        if (explodeOnFade)
            Explode();
    }

    float rotVal;
    rotLeftover = modff(rotSpeed * dt + rotLeftover,&rotVal);
    int newRot = (int)(GetRotation()+rotVal) % 360;
    if (newRot < 0) newRot += 360;
    SetRotation(newRot);

	if ((mTimeForTracking > 0.0f) && (mTargetChooser != NULL)){
		// TODO: lepsze wybieranie celu, jakis mTargetChooser->StillValid() ?
		if ((mTarget == NULL) || (mTarget->IsDead())){
			CPhysical *phys = mTargetChooser->Choose(this);
			CActor *target = NULL;
			if (phys != NULL)
				target = dynamic_cast<CActor*>(phys);
			if (target != NULL){
				mTarget = target->GetSafePointer();
			}
		}
		if (mTarget != NULL){
			TurnTowards(mTarget,dt);
		}
		mTimeForTracking -= dt;
	}

    if (acceleration != 0.0f){
        float v = Maths::Length(GetVelocity()) + acceleration*dt;
        if (v < 0.0f) v = 0.0f;
        SetVelocity(Maths::Normalize(GetVelocity())*v);
    }
}

void CBullet::TurnTowards(Memory::CSafePtr<CActor> tgt, float dt){
	float rot = float( RotationFromVector(GetVelocity()) );
	//warning C4244: 'initializing' : conversion from 'int' to 'float', possible loss of data
	float tgtDir = (float)RotationFromVector(tgt->GetPosition() - GetPosition());
	float turningSharpness = Maths::Abs( rot - tgtDir );

    if ( turningSharpness < 1.0f )
	{
        return;
    }

	float maxTurn = mTurningSpeed * dt;
    float angleByLeft  = tgtDir - rot; while ( angleByLeft < 0.0f )  angleByLeft += 360.0f;
    float angleByRight = rot - tgtDir; while ( angleByRight < 0.0f ) angleByRight += 360.0f;

    if ( angleByLeft < angleByRight)	rot = std::min( angleByLeft, maxTurn );			// w lewo
	else								rot = std::min( angleByRight, maxTurn ) * -1.0f;// w prawo
	float newDir = RotationFromVector(GetVelocity()) + rot;
    while (newDir < 0.0f) newDir += 360.0f;
    while (newDir > 360.0f) newDir -= 360.0f;
	SetVelocity(Maths::Length(GetVelocity()) * RotationToVector(newDir));
	if (rotSpeed == 0){
		//warning C4244: 'argument' : conversion from 'float' to 'int', possible loss of data
		SetRotation((int)newDir);
	}
}

CBullet::~CBullet()
{
    if (meAsSource != NULL){
        meAsSource->Invalidate();
        meAsSource->SetSecondary(source);
    }
}

void CBullet::HandleCollision(CPhysical *phys){
    if (phys)
    {
        if (!(phys->GetCategory() & filter))
            return;
    }

    if ((GetEffect()!=NULL) && (phys!=NULL)){
        EffectSourcePtr src;
        if (this->source == EffectSourcePtr(NULL)){
            if (meAsSource == NULL)
                meAsSource = new CEffectSource(this);
            src = meAsSource;
        } else{
            src = new CEffectSource(this);
            src->SetSecondary(source);
        }
        GetEffect()->Apply(phys,src,context);
    }
    Explode();
    MarkForDelete();
}

CAppliedEffectContainer *CBullet::GetAppliedEffectContainer(){
    return &mAppliedEffects;
}
