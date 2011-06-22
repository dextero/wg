#ifndef CWANDERERSCHEME_H_
#define CWANDERERSCHEME_H_

#include "CAIScheme.h"

class CWandererScheme: public CAIScheme{
public:
    CWandererScheme();
    virtual ~CWandererScheme();
	
    virtual void UpdateAI(CActorAIData *ai, float dt);

    virtual std::string GetName() const;
	virtual void Init(CAISchemeManager &sm);
};

#endif /*CWANDERERSCHEME_H_*/
