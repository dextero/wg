#ifndef CDOOR_H_
#define CDOOR_H_

// Drzwi/brama/blokada

#include <string>
#include <vector>

#include "../CPhysical.h"

class CCondition;
class CEffectHandle;
struct SAnimation;

class CDoor: public CPhysical {
    friend class CPhysicalManager;
public:
    CDoor(const std::wstring& uniqueId);
    virtual ~CDoor();

    enum DoorState{
        dsOpening,
        dsOpen,
        dsClosing,
        dsClosed
    };
protected:
    DoorState mState, mLastState;

    bool mAuto;

    CCondition *mCond;
    int mSpecificCheck;
    CEffectHandle *mOnOpened;

    std::vector<SAnimation*> mAnims;
	
	bool mFirstUpdate;
public:
    virtual void Update(float dt);

    bool GetOpened();
    void SetOpened(bool opened);
    void SetOpenedAuto();

    inline CCondition *GetCondition(){ return mCond; }
    inline void SetCondition(CCondition *cond){ mCond = cond; }

    void SetAnimation(DoorState s, SAnimation *anim);

    bool LetThrough();

    void SetOnOpened(CEffectHandle *effect);
	CEffectHandle* GetOnOpened() { return mOnOpened; }
};

#endif /*CDOOR_H_*/
