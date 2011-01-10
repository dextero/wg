#ifndef _LAIR_GUARDIAN_SCHEME_H_
#define _LAIR_GUARDIAN_SCHEME_H_

#include "CAIScheme.h"

class CLairGuardianScheme: public CAIScheme
{
public:
    CLairGuardianScheme();
    virtual ~CLairGuardianScheme();
	
    virtual void UpdateAI(CActorAIData *ai, float dt);
    
    virtual std::string GetName() const;    
	virtual void Init(CAISchemeManager &sm);
};

#endif /*_LAIR_GUARDIAN_SCHEME_H_*/
