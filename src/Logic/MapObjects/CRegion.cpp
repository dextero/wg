#include "CRegion.h"
#include "../CPlayer.h"
#include "../../Rendering/Primitives/CPrimitivesDrawer.h"
#include "../../Map/CMapManager.h"
#include "../../Rendering/Primitives/CPrimitiveClass.h"
#include "../Conditions/CCondition.h"
#include "../Effects/CEffectHandle.h"

#include <cstdio>

CPrimitiveClass *pc = NULL;

CDynamicRegion::CDynamicRegion(const std::wstring& uniqueId):
CPhysical(uniqueId),
mNextMap(NULL),
mNextMapRegion(NULL),
mDisplay(false),
mCond(NULL),
mTriggerTime(0.0f),
mEffectOnEnter(NULL),
mRotation(0.0f),
mDescriptorId(-1),
mSelectedToErase(false)
{
    SetCategory(PHYSICAL_REGION);
}

CDynamicRegion::~CDynamicRegion(){
    if (mCond){
        mCond->Unregister();
        mCond->Reset();
    }
}

void CDynamicRegion::SetCondition(CCondition *cond){
    if (mCond!= NULL){
        mCond->Unregister();
        mCond->Reset();
    }
    mCond = cond;
    if (mCond!=NULL){
        mCond->Register();
        mCond->Reset();
    }
}

void CDynamicRegion::Update(float dt){
    if (mDisplay){
        sf::Vector2f topLeft = GetPosition()-sf::Vector2f(GetCircleRadius(),GetCircleRadius());
        sf::Vector2f bottomRight = GetPosition()+sf::Vector2f(GetCircleRadius(),GetCircleRadius());

#ifndef __EDITOR__
        if (pc == NULL)
            pc = gPrimitivesDrawer.GetClass("region");
#else
        pc = gPrimitivesDrawer.GetClass(mSelectedToErase ? "region-hl" : "region");
#endif

        gPrimitivesDrawer.DrawRect(pc,topLeft,bottomRight, 0.05f);
    }
    if (mTriggerTime > 0.0f){
        mTriggerTime -=dt;
        if (mTriggerTime < 0.0f)
            mTriggerTime = 0.0f;
    }
}


void CDynamicRegion::Trigger(CPlayer *player){

    //tox, robie obejscie na gazomierzu: jesli ten trigger byl odpalany w ciagu ostatnich
    //0.5 sekundy, to nie probuj go nawet odpalic ponownie. Patrz #1122
    if (mTriggerTime > 0.0f) {
        return;
    }

    // najpierw sprawdzamy warunek...
    if (mCond != NULL){
        if (!mCond->SpecificCheck()) // warunek na aktywnosc nie spelniony 
            return;
    }
    // region bedzie "odpalony" przez pol sekundy po wejsciu na niego gracza
    mTriggerTime = 0.5f;
    // moze zmieniamy mape?
    if (mNextMap != NULL && !mNextMap->empty()) {
        gMapManager.EnterMap(*mNextMap, *mNextMapRegion);
        return;
    }
    // a moze... ? :)
    if (mEffectOnEnter)
    {
        mEffectOnEnter->PerformAt(GetPosition());
        return;
    }
    
}

bool CDynamicRegion::IsTriggered(){ 
    if (mTriggerTime == 0.0f) {
        return false;
    } else {
        if (mCond != NULL && !mCond->SpecificCheck()) {
            return false;
        }
        return true;
    }
}

void CDynamicRegion::SetEffectOnEnter(CEffectHandle *effect)
{
    mEffectOnEnter = effect;
}

