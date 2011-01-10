#ifndef CIDLEMOVEMENTSCHEME_H_
#define CIDLEMOVEMENTSCHEME_H_

#include "CAIScheme.h"

class CIdleMovementScheme: public CAIScheme{
protected:
	CAIScheme *mLairGuardian;
	CAIScheme *mWanderer;
	// TODO: idea: Patrol-scheme
public:
    CIdleMovementScheme();
    virtual ~CIdleMovementScheme();
	
    virtual void UpdateAI(CActorAIData *ai, float dt);

    virtual std::string GetName() const;
	virtual void Init(CAISchemeManager &sm);
};

#endif /*CIDLEMOVEMENTSCHEME_H_*/
