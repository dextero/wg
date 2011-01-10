#ifndef __CAPPLIEDEFFECT_H__
#define __CAPPLIEDEFFECT_H__

/**
 * Liosan, 12.03
 * Struktura reprezentujaca zastosowanie efektu do aktora
 * 
 * Liosan, 27.06.09 - dodalem mechanizm odsmiecania pamieci + zrodla
 */

#include "CAppliedEffectTypes.h"
#include "CExecutionContext.h"
#include "CFocusObject.h"

class CEffectHandle;
struct SDisplayableEffect;

class CAppliedEffect: public Memory::CRefPoolable{
private:
    static int InstancesCount;
public:
    static inline int GetInstanceCount(){ return InstancesCount; }
protected:
    CAppliedEffect();
    ~CAppliedEffect(){InstancesCount--;}

    friend class Memory::CPool<CAppliedEffect>;

    bool isValid;
public:
    static CAppliedEffect *Alloc();

    float durationLeft;
    float reapplyLeft;
    float reapplyTime;
    int offset;
    EffectSourcePtr source;
    EffectSourcePtr meAsSource;
    ExecutionContextPtr context;
    FocusObjectPtr focus;
    bool brokenFocus; // czy uzyto tego efektu do zlamania koncentracji; kazdy efekt tylko raz
    SDisplayableEffect *displayableEffectHandle;

    inline bool IsApplied(){ return !(durationLeft!=durationLeft); } // NaN <-> jednokrotna aplikacja

    inline bool IsValid(){ return isValid; }

    void Invalidate();
};

#endif /*__CAPPLIEDEFFECT_H__*/
