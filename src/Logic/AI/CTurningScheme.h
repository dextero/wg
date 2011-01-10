#ifndef CTURNINGSCHEME_H_
#define CTURNINGSCHEME_H_

#include "CAIScheme.h"

class CTurningScheme: public CAIScheme{
public:
    CTurningScheme();
    virtual ~CTurningScheme();
	
    virtual void UpdateAI(CActorAIData *ai, float dt);

    virtual std::string GetName() const;
	virtual void Init(CAISchemeManager &sm);
};

#endif /*CWANDERERSCHEME_H_*/
