#ifndef __CEFFECTSOURCE_H__
#define __CEFFECTSOURCE_H__

#include <string>
#include "../../Utils/Memory/CRefPoolable.h"
#include "../../Utils/Memory/RefCntPtr.h"
#include "CAppliedEffectTypes.h"
#include "CAppliedEffect.h"

/**
 * Liosan, 26.06
 * zrodlo efektu - potrzebne przy umiejetnosciach pasywnych, lancuchowaniu, wybuchach itp
 * Radze uzywac przez inteligentny wskaznik - EffectSourcePtr
 */

class CBullet;
class CAbility;
class CDetector; // do eksplozji
//class CAppliedEffect; // jest #include "CAppliedEffectTypes.h"
class CActor;

enum EEffectSourceType{
    estNone, // zaden lub nieznany
    estNextSource,
    estBullet,
    estAbility,
    estExplosion,
    estAppliedEffect,
    estActor,
    estDelayingAbility, // te dwa trzeba ustawiac recznie, to i tak jest dokladnie jeden przypadek uzycia ;)
    estCastingAbility,
    estGenericPhysical
};

class CEffectSource : public Memory::CRefPoolable {
private:
    friend class CSource;

    static int InstancesCount;

    bool isValid;
public:
    static inline int GetInstanceCount(){ return InstancesCount; }

    class CSource{
    private:
        friend class CEffectSource;
        void PrintToStderr(int indent);
    public:
        EEffectSourceType type;
        union{
            void *void_ptr;
            CBullet *bullet;
            CAbility *ability;
            CDetector *detector;
            CActor *actor;
            CPhysical *physical; // nie uzywane, ale to taki reinterpret_cast ;)
        };
        // powiem tak: umiem zrobic tak, zeby te non-pody wrzucic do unii. Ale to jest
        // tak brzydkie obejscie tej teoretycznej niemozliwosci, i tak niebezpieczne jesli sie
        // rypne, ze wole wydac te dodatkowe 4 bajty ;)
        CAppliedEffectPtr appliedEffect;
        EffectSourcePtr nextSource;

        inline bool ContainsPhysical(){
            switch (type){
                case estNextSource:
                case estBullet:
                case estExplosion:
                case estActor:
                case estGenericPhysical:
                    return true;
                default:
                    return false;
            }
        }

        inline CSource(){
            type = estNone;
            void_ptr = NULL;
        }
        inline CSource(CBullet *bullet){ Set(bullet); }
        inline CSource(CAbility *ability){ Set(ability); }
        inline CSource(CDetector *detector){ Set(detector); }
        inline CSource(CAppliedEffectPtr appliedEffect){ Set(appliedEffect); }
        inline CSource(CActor *actor){ Set(actor); }
        inline void Set(CBullet *bullet){
            if (appliedEffect !=NULL) appliedEffect = NULL;
            if (nextSource !=NULL) nextSource = NULL;
            type = estBullet;
            this->bullet = bullet;
        }
        inline void Set(CAbility *ability){
            if (appliedEffect !=NULL) appliedEffect = NULL;
            if (nextSource !=NULL) nextSource = NULL;
            type = estAbility;
            this->ability = ability;
        }
        inline void Set(CDetector *detector){
            if (appliedEffect !=NULL) appliedEffect = NULL;
            if (nextSource !=NULL) nextSource = NULL;
            type = estExplosion;
            this->detector = detector;
        }
        inline void Set(CAppliedEffectPtr appliedEffect){
            if (nextSource !=NULL) nextSource = NULL;
            type = estAppliedEffect;
            this->appliedEffect = appliedEffect;
        }
        inline void Set(CActor *actor){
            if (appliedEffect !=NULL) appliedEffect = NULL;
            if (nextSource !=NULL) nextSource = NULL;
            type = estActor;
            this->actor = actor;
        }
        inline void Set(EffectSourcePtr other){
            if (appliedEffect !=NULL) appliedEffect = NULL;
            type = estNextSource;
            nextSource = other;
        }
    };

    CSource primary;
    CSource secondary;

    explicit inline CEffectSource():isValid(true){ InstancesCount++;}
    explicit inline CEffectSource(EEffectSourceType est, void *ptr){
        InstancesCount++;
        primary.type = est;
        primary.void_ptr = ptr;
        isValid = true;
    }
    inline CEffectSource(CBullet *bullet):isValid(true), primary(bullet){InstancesCount++;}
    inline CEffectSource(CAbility *ability):isValid(true), primary(ability){InstancesCount++;}
    inline CEffectSource(CDetector *detector):isValid(true), primary(detector){InstancesCount++;}
    inline CEffectSource(CAppliedEffectPtr appliedEffect):isValid(true), primary(appliedEffect){InstancesCount++;}
    inline CEffectSource(CActor *actor):isValid(true), primary(actor){InstancesCount++;}
    ~CEffectSource(){ InstancesCount--; }

    // wzorzec "Builder": dzieki temu, ze to zwraca this, mozna zrobic np tak:
    // Foo(Bar,(new CEffectSource(bullet))->SetSecondary(ability));
    inline CEffectSource *SetSecondary(CBullet *bullet){ secondary.Set(bullet); return this; }
    inline CEffectSource *SetSecondary(CAbility *ability){ secondary.Set(ability); return this; }
    inline CEffectSource *SetSecondary(CDetector *detector){ secondary.Set(detector); return this; }
    inline CEffectSource *SetSecondary(CAppliedEffectPtr appliedEffect){ secondary.Set(appliedEffect); return this; }
    inline CEffectSource *SetSecondary(CActor *actor){ secondary.Set(actor); return this; }
    inline CEffectSource *SetSecondary(EffectSourcePtr other){ secondary.Set(other); return this; }

    inline bool IsValid(){ return isValid; }
    inline void Invalidate() { isValid = false; }

    CPhysical *DeterminePhysicalSource();
    void PrintToStderr();
private:
    void PrintToStderr(int indent);
};

#endif /*__CEFFECTSOURCE_H__*/
