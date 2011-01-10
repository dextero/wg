#ifndef __CSTATS_H__
#define __CSTATS_H__

#include "CAspectBatch.h"
#include "../../Utils/CXml.h"

/**
 * Liosan, 27.06.09
 * Klasa zawierajaca aspekty, hp, mane itp aktora.
 * Wydzielenie jej ma na celu uproszczenie CActor
 * i ew. umozliwienie tworzenia obiektow "niszczalnych" nie bedacych aktorami
 */

class CPhysical;

class CStats{
private:
    CPhysical *mAttached;

    // aktualne wartosci aspektow (sila, szybkosc, maxHP itp)
    CAspectBatch mCurrAspects;
    // bazowe (referencyjne) wartosci aspektow
    CAspectBatch mBaseAspects;
    
    float mHP;
    float mMana;
public:
    CStats(CPhysical *attached);
    ~CStats();

    void LerpContent(CStats *min,CStats *max, float f);
    void Load(CXml &xml,const std::string &attr);

    inline CPhysical *GetPhysical(){ return mAttached; }
    inline void SetPhysical(CPhysical *attached){ mAttached = attached; }

    void DoDamage(float dmg);
    inline void DoHeal(float heal) { SetHP(mHP + heal); }
    bool TryDrainHealth(float val);

    inline float GetBaseAspect(EAspect a) { return mBaseAspects[a]; }
    void SetBaseAspect(EAspect a, float v);

    inline float GetCurrAspect(EAspect a) { return mCurrAspects[a]; }
    void SetCurrAspect(EAspect a, float v);

    inline const CAspectBatch &GetAllCurrAspects(){ return mCurrAspects; }

    inline float GetHP() { return mHP; }
    void SetHP(float hp);

	inline float GetMana() { return mMana; }
	inline void SetMana(float mana) { 
        mMana = mana; 
        if (mMana > GetCurrAspect(aMaxMana)) 
            mMana = GetCurrAspect(aMaxMana); 
        else 
            if (mMana < 0.f) mMana = 0.f; 
    }
    
    void RestoreMana(float val);
    void DrainMana(float val);
    bool TryDrainMana(float val);
};

#endif /*__CSTATS_H__*/
