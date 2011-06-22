#ifndef _QUICK_CHASER_SCHEME_H_
#define _QUICK_CHASER_SCHEME_H_

#include "CAIScheme.h"
#include "CShortestPaths.h"

class CQuickChaserScheme: public CAIScheme
{
private:
	static CShortestPaths * mNarrowPathsToPlayer0;
	static CShortestPaths * mWidePathsToPlayer0;
	static CShortestPaths * mNarrowPathsToPlayer1;
	static CShortestPaths * mWidePathsToPlayer1;
protected:
	CAIScheme *mStupidChaserScheme;
	CAIScheme *mIdleChaserScheme;
public:
    CQuickChaserScheme();
    virtual ~CQuickChaserScheme();
	
    virtual void UpdateAI(CActorAIData *ai, float dt);
    
    virtual std::string GetName() const;    	
	virtual void Init(CAISchemeManager &sm);
};

#endif /*_QUICK_CHASER_SCHEME_H_*/

