#ifndef _RANGED_CASTER_SCHEME_H_
#define _RANGED_CASTER_SCHEME_H_

#include "CAIScheme.h"

class CRangedCasterScheme: public CAIScheme
{
protected:
	CAIScheme *mCombatScheme;
	CAIScheme *mMovementScheme;
	CAIScheme *mChaserScheme;
	CAIScheme *mTurningScheme;
public:
    CRangedCasterScheme();
    virtual ~CRangedCasterScheme();
	
    virtual void UpdateAI(CActorAIData *ai, float dt);

    virtual std::string GetName() const;
	virtual void Init(CAISchemeManager &sm);
};

#endif /*_RANGED_CASTER_SCHEME_H_*/
