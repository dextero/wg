#ifndef CACTOR_H_
#define CACTOR_H_

#include "CPhysical.h"
#include "Stats/EAspect.h"
#include "Stats/EDamageType.h"
#include "Stats/CStats.h"
#include "Stats/CBleeder.h"
#include "CCallbackDispatcher.h"
#include "Abilities/CAbilityPerformer.h"
#include "Effects/CAppliedEffectContainer.h"
#include "Abilities/CSummonContainer.h"
#include "../Utils/Memory/CSafePtrable.h"

#include <cstdio>
#include <vector>
#include <map>

/**
 * Klasa reprezentujaca wrogow, gniazda, gracza (i ew. NPC? do rozwazenia)
 * 
 * Przechowuje rozne cechy potwora lub gracza - maksymalna szybkosc, sile,
 * liczbe punktow zycia, dostepne ataki czy zestaw animacji
 *
 * Liosan, 03.07 - wyekstrahowalem zaaplikowane efekty do oddzielnej klasy
 * - CAppliedEffectContainer. No, to teraz se mozemy skladac klasy z klockow...
 *
 * Liosan, 26.06.09 - wyekstrahowalem statsy do oddzielnej klasy - CStats
 *
 * Liosan, 12.03 - przerobiono wszystkie istotne cechy (speed, maxHP itp)
 * na wektor aspects - sa stablicowane, ich przetwarzanie jest ujednolicone
 *
 * Liosan, 31.01 - dodano metode RegisterEventOnDeath - rejestrowanie funkcji, ktore
 * zostana wywolane po smierci obiektu. Cel: rozwiazanie problemu
 * wskaznikow na zniszczone obiekty
 */

class CAnimSet;
class CAbility;
class CActorController;
class CActorTemplate;
class CCondition;
class CLair;
struct SAnimation;

namespace System { namespace Resource {
	class CSound;
}}

class CActor : public CPhysical, public Memory::CSafePtrable<CActor>
{
    friend class CPhysicalManager;
    friend class CActorController;
protected:
	CActor(const std::wstring &uniqueId);
	virtual ~CActor();
//--------------------------------------------- Glowne skladowe - kontroler, stany, animset
    CActorController *mController;

    float mAnimationTime;
    float mAnimationTimeLeft;

    CAnimSet *mAnimSet;

    float mXPValue; // ile sie za tego Zenka XPa dostanie
public:
    inline float GetXPValue(){ return mXPValue; }
    inline void SetXPValue(float val){ mXPValue = val; }

    enum EActorMoveState { 
        msStanding, 
        msWalking, 
        msTurning,
        msStrafing };
    enum EActorSpawnState{
        ssSpawning,
        ssAlive,
        ssAsleep,
        ssDying };
protected:
    EActorMoveState mMoveState;
    EActorSpawnState mSpawnState;

    CCondition *mCondition;
    
    // czy aktor jest ogluszony
    // int zamiast boolean, zeby moglo byc kilka zrodel ogluszenia
    int mStunLevel;
	// czy aktor rzuca mana zamiast zyciem
    // int zamiast boolean - j.w.
	int mHealthcastLevel;
	// ilosc, o ktora zwiekszyc poziom nastepnego czaru
	int mNextAbilityLevelBonus;
public:
    inline CActorController * GetActorController() { return mController; }
    inline CCondition *GetCondition() { return mCondition; }
    inline void SetCondition(CCondition *cond){ mCondition = cond; }

    inline EActorMoveState GetMoveState(){ return mMoveState; }
    inline EActorSpawnState GetSpawnState(){ return mSpawnState; }
    CAnimSet *GetAnimSet(){ return mAnimSet; }
    void SetAnimSet(CAnimSet *as);
    void UpdateAnimations(float dt);
    
    // zabija aktora, wywolujac odpowiednie czynnosci z tym zwiazane
	virtual void Kill();

    inline void SetStun(bool s){ if (mNoStun) return; mStunLevel += s ? 1 : -1; }
    inline int GetStun(){ return mStunLevel; }

	inline void SetHealthcast(bool h){ mHealthcastLevel += h ? 1 : -1; }
	inline int GetHealthcast(){ return mHealthcastLevel; }
	
	inline void IncreaseNextAbilityLevel(int cnt){ mNextAbilityLevelBonus = cnt; }
	inline int ConsumeNextAbilityLevel() { int tmp = mNextAbilityLevelBonus; mNextAbilityLevelBonus = 0; return tmp; }
	inline int ReadNextAbilityLevel() { return mNextAbilityLevelBonus; }
//--------------------------------------------- Aspekty
protected:
    CStats mStats;
    CAppliedEffectContainer mAppliedEffectContainer;
    CBleeder mBleeder;
public:
    virtual CStats *GetStats();
    virtual CAppliedEffectContainer *GetAppliedEffectContainer();
    virtual CBleeder *GetBleeder();

    // w aktorach radius jest powiazany z aspektem size - stad virtual
    virtual void SetBoundingCircle( float radius );

    virtual void Update( float dt );
    // dwie oddzielne metody dla wygody - DoDamage wiaze sie z krwawieniem, odpornosciami itp
    void DoDamage(float dmg, DamageTypes::EDamageType dt = DamageTypes::dtPhysical, EffectSourcePtr source = NULL);
    // UWAGA: powoduje wyswietlenie zielonego napisu; jesli tego nie chcesz, uzyj GetStats()->DoHeal
    void DoHeal(float heal);
    inline float GetHP(){ return mStats.GetHP(); }
    
    // stopnie / s
    inline float GetMaxTurnSpeed(){ return mStats.GetCurrAspect(aTurnSpeed); }

    inline sf::Vector2f GetForce() { return mForce; }
    inline void SetForce(sf::Vector2f force) { mForce = force / mStats.GetCurrAspect(aMass); mForceTime = 1.0f;  }
    void ApplyForce(sf::Vector2f force);

    bool IsNoStun() { return mNoStun; }
    void SetNoStun(bool noStun) { mNoStun = noStun; }
protected:
// ------------------------------------------- Umiejetnosci
    sf::Vector2f mForce;
    float mForceTime;
    bool mNoStun;
    
    SAnimation *mLastAnimation;
    CAbilityPerformer mAbilityPerformer;
public:
    inline CAbilityPerformer &GetAbilityPerformer(){ return mAbilityPerformer; }
	virtual CPinnedAbilityBatch *GetPinnedAbilityBatch();
// ------------------------------------------ Dzwieki
protected:
    std::map< std::string, std::vector<System::Resource::CSound*> > mSound;
public:
    inline void AddSound(std::string type, System::Resource::CSound* snd)     { mSound[type].push_back(snd); }
    inline std::vector<System::Resource::CSound*>& GetSounds(std::string type)  { return mSound[type]; }

	inline void AddAttackSound( System::Resource::CSound* snd )			{ mSound["attack"].push_back(snd); }
	inline std::vector<System::Resource::CSound*>& GetAttackSounds()	{ return mSound["attack"]; }

//------------------------------------------- Rejestrowanie callbackow
private:
    CCallbackDispatcher mCallbackDispatcher;
public:
    inline CCallbackDispatcher *GetCallbackDispatcher(){ return &mCallbackDispatcher; }
//------------------------------------------- Ustawianie gniazda z ktorego pochodzi potwor
private:
	CLair * mLair;
public:
	inline void SetLair( CLair* lair ) { mLair = lair; }
	inline CLair* GetLair() { return mLair; }
    
    virtual bool IsDead();
protected:
    CSummonContainer mSummonContainer;
};

#endif /*CACTOR_H_*/
