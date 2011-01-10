#ifndef __CBULLET_H__
#define __CBULLET_H__

#include "CPhysical.h"
#include "Effects/CEffectSource.h"
#include "Effects/CExecutionContext.h"
#include "Effects/CAppliedEffectContainer.h"
#include "../Utils/Memory/CSafePtr.h"

class CEffectHandle;
class CActor;
class CTargetChooser;

class CBullet : public CPhysical
{
private:
    CAppliedEffectContainer mAppliedEffects;
protected:
    friend class CPhysicalManager;
    CBullet(const std::wstring& uniqueId);
    virtual ~CBullet();

    void Explode();
public:
	inline CEffectHandle *GetEffect()				{ return mEffect; }
	inline void	SetEffect(CEffectHandle *effect)	{ mEffect = effect; }

    inline CEffectHandle *GetExplosionEffect()           { return mExpl; }
	inline void	SetExplosionEffect(CEffectHandle *effect){ mExpl = effect; }

    virtual void Update(float dt);
    float TTL;
    float range;
    EffectSourcePtr source;
    EffectSourcePtr meAsSource;
    int rotSpeed;
    ExecutionContextPtr context;
    float acceleration;
    int explodeOnFade;
    int filter; //tox, 26 sierpnia - patrz komentarz w BulletEmiterze.h

    void HandleCollision(CPhysical *phys);

    virtual CAppliedEffectContainer *GetAppliedEffectContainer();

	// dla pociskow samonaprowadzajacych
	float mTimeForTracking;
	float mTurningSpeed;
	Memory::CSafePtr<CActor> mTarget;
	CTargetChooser *mTargetChooser;
private:
    float rotLeftover;
	CEffectHandle *mEffect;
	CEffectHandle *mExpl;

	void TurnTowards(Memory::CSafePtr<CActor> tgt,float dt);
};

#endif

