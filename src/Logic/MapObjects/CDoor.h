#ifndef CDOOR_H_
#define CDOOR_H_

// Drzwi/brama/blokada

#include <string>
#include <vector>
#include <map>

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
        dsOpened,
        dsClosed
    };
protected:
    DoorState mState;
    std::map<DoorState, SAnimation*> mAnims;
	
public:
    virtual void Update(float dt);

    bool IsOpened();
    void SetState(DoorState ds);
    void SetAnimation(DoorState s, SAnimation *anim);
};

#endif /*CDOOR_H_*/
