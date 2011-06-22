#ifndef _GENERIC_COMBAT_SCHEME_H_
#define _GENERIC_COMBAT_SCHEME_H_

#include "CAIScheme.h"

class CGenericCombatScheme: public CAIScheme
{
public:
    CGenericCombatScheme();
    virtual ~CGenericCombatScheme();
	
    virtual void UpdateAI(CActorAIData *ai, float dt);

    virtual std::string GetName() const;
	virtual void Init(CAISchemeManager &sm);
};

#endif /*_GENERIC_COMBAT_SCHEME_H_*/
