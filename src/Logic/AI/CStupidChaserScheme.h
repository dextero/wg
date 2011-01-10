#ifndef _CSTUPIDCHASERSCHEME_H_
#define _CSTUPIDCHASERSCHEME_H_

#include "CAIScheme.h"

class CStupidChaserScheme: public CAIScheme{
public:
    CStupidChaserScheme();
    virtual ~CStupidChaserScheme();
	
    virtual void UpdateAI(CActorAIData *ai, float dt);

    virtual std::string GetName() const;
	virtual void Init(CAISchemeManager &sm);
};

#endif /*_CSTUPIDCHASERSCHEME_H_*/
