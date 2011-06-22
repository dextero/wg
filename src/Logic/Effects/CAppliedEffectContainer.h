#ifndef __CAPPLIEDEFFECTCONTAINER_H__
#define __CAPPLIEDEFFECTCONTAINER_H__

#include "CAppliedEffect.h"
#include "CEffectSource.h"

/**
 * Liosan, 03.07.09
 * Klasa zawierajaca obiekty AppliedEffect dotyczace physicala
 * Wydzielenie jej ma na celu uproszczenie CActor
 * i ew. umozliwienie tworzenia obiektow "emiterow" - np. pulapek
 */

class CPhysical;

class CAppliedEffectContainer{
private:
    CPhysical *mAttached;
    friend class CEffectManager;
public:
    CAppliedEffectContainer(CPhysical *attached);
    ~CAppliedEffectContainer();

    inline CPhysical *GetPhysical(){ return mAttached; }
    inline void SetPhysical(CPhysical *attached){ mAttached = attached; }
private:
    struct SAppliedEffectData{
        CAppliedEffectPtr effect;
        float unbledDamage;
        float bleedDelay;

        SAppliedEffectData():
            effect(NULL),
            unbledDamage(0.0f),
            bleedDelay(1.0f){}
    };

    // wektor effectow majacych aktualnie wplyw na aktora
    std::vector<SAppliedEffectData> appliedEffects;
    void BleedFromAppliedEffects(float dt);
	bool mLocked;
public:
    bool TryCumulateUnbledDamage(float dmg,CAppliedEffectPtr ae);
    void Update( float dt );
    
    // dodaje efekt do listy efektow wplywajacych na aktora
    void AddEffect(CAppliedEffectPtr ae);
    // usuwa efekty wg zrodla
    void RemoveEffects(EffectSourcePtr source);
    // usuwa efekty wg offsetu (uzywane przez efekty non-cumulative)
    bool RemoveEffects(int offsetFrom, int offsetTo);
};


#endif /*__CAPPLIEDEFFECTCONTAINER_H__*/
