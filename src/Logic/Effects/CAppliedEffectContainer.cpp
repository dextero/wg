#include <cassert>

#include "CAppliedEffectContainer.h"
#include "../CPhysical.h"
#include "../../Utils/MathsFunc.h"
#include "../Effects/CEffectManager.h"
#include "../Effects/CFocusObject.h"
#include "../Stats/CBleeder.h"

CAppliedEffectContainer::CAppliedEffectContainer(CPhysical *attached):
mAttached(attached),mLocked(false){
    assert(mAttached!=NULL);
}

CAppliedEffectContainer::~CAppliedEffectContainer(){
    for (unsigned int i = 0; i < appliedEffects.size(); i++)
        appliedEffects[i].effect->Invalidate();
}

void CAppliedEffectContainer::BleedFromAppliedEffects(float dt){
    CBleeder *bleeder = mAttached->GetBleeder();
    if (bleeder == NULL) return;

    for (unsigned int i = 0; i < appliedEffects.size(); i++){
        if (appliedEffects[i].unbledDamage != 0.0f){
            appliedEffects[i].bleedDelay -= dt;
            if (appliedEffects[i].bleedDelay < 0.0f){
                appliedEffects[i].bleedDelay = 1.0f;
                bleeder->Bleed(appliedEffects[i].unbledDamage,appliedEffects[i].effect->meAsSource);
                appliedEffects[i].unbledDamage = 0.0f;
            }
        }
    }
}

bool CAppliedEffectContainer::TryCumulateUnbledDamage(float dmg,CAppliedEffectPtr ae){
    for (unsigned int i = 0; i < appliedEffects.size(); i++){
        if (appliedEffects[i].effect == ae){
            appliedEffects[i].unbledDamage+=dmg;
            return true;
        }
    }
    return false;
}

void CAppliedEffectContainer::AddEffect(CAppliedEffectPtr ae){
    SAppliedEffectData aed;
    ae->reapplyLeft = ae->reapplyTime;
    aed.effect = ae;
    appliedEffects.push_back(aed);
    ae->meAsSource = new CEffectSource(ae);
    ae->meAsSource->SetSecondary(ae->source);
}

void CAppliedEffectContainer::RemoveEffects(EffectSourcePtr source){
    unsigned int i = 0;
    while (i < appliedEffects.size()){
        // szybkie usuwanie - bierzemy ostatni element z konca ALBO zwiekszamy licznik
        if (appliedEffects[i].effect->source == source){ 
            SAppliedEffectData aed = appliedEffects[i];
            appliedEffects[i]=appliedEffects[appliedEffects.size()-1];
            appliedEffects.pop_back();
            gEffectManager.Unapply(mAttached,aed.effect);
            aed.effect->Invalidate();
        } else i++;
    }
}

bool CAppliedEffectContainer::RemoveEffects(int offsetFrom, int offsetTo){
    unsigned int i = 0;
    while (i < appliedEffects.size()){
        // szybkie usuwanie - bierzemy ostatni element z konca ALBO zwiekszamy licznik
		int off = appliedEffects[i].effect->offset;
        if ((off >= offsetFrom) && (off <= offsetTo)){ 
			if (mLocked)
				return false; // powiem Wam, ze jak sie usuwa efekt, ktory sie wykonuje, to jest to zle
			//int dbg = appliedEffects[i].effect->offset;
			//fprintf(stderr,"removing effect offset %d, i is %u, size is %u, physical at %p\n",dbg,i,appliedEffects.size(),mAttached);
            SAppliedEffectData aed = appliedEffects[i];
            appliedEffects[i]=appliedEffects[appliedEffects.size()-1];
            appliedEffects.pop_back();
            gEffectManager.Unapply(mAttached,aed.effect);
            aed.effect->Invalidate();
			//fprintf(stderr,"removed effect offset %d, i is %u, size is %u, physical at %p\n",dbg,i,appliedEffects.size(),mAttached);
        } else i++;
    }
	return true;
}

void CAppliedEffectContainer::Update(float dt){
    float r;
    unsigned int i = 0;
	mLocked = true;
    while (i < appliedEffects.size()){
		//int dbg = appliedEffects[i].effect->offset;
		//fprintf(stderr,"last offset is %d, i is %u, size is %u, physical at %p\n",dbg,i,appliedEffects.size(),mAttached);
        r = appliedEffects[i].effect->reapplyTime;
        if (r==0.0f)
            gEffectManager.Perform(mAttached,appliedEffects[i].effect,dt);
        else {
            appliedEffects[i].effect->reapplyLeft-=dt;
            while (appliedEffects[i].effect->reapplyLeft<=0.0f){
                appliedEffects[i].effect->reapplyLeft+=r;
				//fprintf(stderr,"loop-performing offset %d, i is %u, size is %u, physical at %p\n",dbg,i,appliedEffects.size(),mAttached);
                gEffectManager.Perform(mAttached,appliedEffects[i].effect,r);
            }
        }
        appliedEffects[i].effect->durationLeft-=dt;
        bool unfocus = false;
        if (appliedEffects[i].effect->focus != NULL)
            if (!(appliedEffects[i].effect->focus->StillValid()))
                unfocus = true;
        // szybkie usuwanie - bierzemy ostatni element z konca ALBO zwiekszamy licznik
        if ((appliedEffects[i].effect->durationLeft <= 0.0f) || unfocus){
            SAppliedEffectData ae = appliedEffects[i];
            appliedEffects[i]=appliedEffects[appliedEffects.size()-1];
            appliedEffects.pop_back();
            gEffectManager.Unapply(mAttached,ae.effect);
            ae.effect->Invalidate();
        } else i++;
    }
    BleedFromAppliedEffects(dt);
	mLocked = false;
}
