#include "CDoor.h"
#include "../../Rendering/ZIndexVals.h"
#include "../../Rendering/CDisplayable.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../CPhysicalManager.h"
#include "../Conditions/CCondition.h"
#include "../Effects/CEffectHandle.h"

#include <sstream>

CDoor::CDoor(const std::wstring& uniqueId) :
CPhysical(uniqueId),
mState(dsClosed)
{
    SetZIndex(Z_DOOR);
}

CDoor::~CDoor(){
    //kto czysci animacje z mAnims[] ? AnimationManager?
}

void CDoor::Update(float dt){
    CPhysical::Update(dt);
}

bool CDoor::IsOpened() {
    return mState == dsOpened;
}

void CDoor::SetState(DoorState ds) {
    mState = ds;
    CPhysical::SetAnimation(mAnims[ds]);
}

void CDoor::SetAnimation(DoorState s, SAnimation *anim){
    mAnims[s] = anim;
}

