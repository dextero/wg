#ifndef _MELEE_CHASER_SCHEME_H_
#define _MELEE_CHASER_SCHEME_H_

#include "CAIScheme.h"

class CMeleeChaserScheme: public CAIScheme
{
protected:
	CAIScheme *mCombatScheme;
	CAIScheme *mMovementScheme;
	CAIScheme *mChaserScheme;
public:
    CMeleeChaserScheme();
    virtual ~CMeleeChaserScheme();
    
    virtual void RegisterAI(Memory::CSafePtr<CActorAI> ai);
    virtual void UnregisterAI(Memory::CSafePtr<CActorAI> ai);
	
    virtual void UpdateAI(CActorAIData *ai, float dt);

    virtual std::string GetName() const;    
	virtual void Init(CAISchemeManager &sm);
};

#endif /*_MELEE_CHASER_SCHEME_H_*/
