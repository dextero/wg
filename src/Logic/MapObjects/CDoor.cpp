#include "CDoor.h"
#include "../../Rendering/ZIndexVals.h"
#include "../../Rendering/CDisplayable.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../CPhysicalManager.h"
#include "../Conditions/CCondition.h"
#include "../Effects/CEffectHandle.h"

CDoor::CDoor(const std::wstring& uniqueId) :
CPhysical(uniqueId),
mState(dsClosed),
mLastState(dsOpen),
mAuto(true),
mCond(NULL),
mSpecificCheck(CCondition::sctIs),
mOnOpened(NULL),
mFirstUpdate(true)
{
    SetZIndex(Z_DOOR);
    for(int i = 0; i < 4; i++)
        mAnims.push_back(NULL);
}

CDoor::~CDoor(){
}

void CDoor::Update(float dt){
    CPhysical::Update(dt);
    if ((mCond!=NULL) && mAuto){
        if (mCond->SpecificCheck())
            mState = dsOpen;
        else
            mState = dsClosed;
    }
    if (mState != mLastState){
        if (mState == dsOpen)
            if (mOnOpened!=NULL)
                mOnOpened->PerformAt(GetPosition());
        mLastState = mState;
        if (mAnims[mState]!=NULL)
            CPhysical::SetAnimation(mAnims[mState]);
    }
	if (mFirstUpdate)
		CPhysical::SetAnimation(mAnims[mState]);
	mFirstUpdate = false;
}

void CDoor::SetAnimation(DoorState s, SAnimation *anim){
    mAnims[s] = anim;
}

bool CDoor::LetThrough(){
    // na razie niczym sie nie rozni, potem ew. moze byc przepuszczanie
    // wybranych typow obiektow albo przepuszczanie w polowie animacji
    // otwierania
    return mState == dsOpen;
}

bool CDoor::GetOpened(){
    return mState == dsOpen;
}

void CDoor::SetOpened(bool opened){
    mAuto = false;
    mState = opened ? dsOpen : dsClosed; // dsOpen; - haha, bardzo smieszne...
}

void CDoor::SetOpenedAuto(){
    mAuto = true;
}

void CDoor::SetOnOpened(CEffectHandle *effect){
    mOnOpened = effect;
}

