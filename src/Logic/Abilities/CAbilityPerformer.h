#ifndef __CABILITYPERFORMER_H__
#define __CABILITYPERFORMER_H__

/**
 * Liosan, 16.05:
 * klasa pomocnicza, obslugujaca liste umiejetnosci aktora,
 * ich stan, czas rzucania, delay i cooldown.
 */

#include <string>
#include <vector>
#include "../Effects/CEffectSource.h"
#include "../Effects/CFocusObject.h"
#include "SAbilityInstance.h"
#include "EAbilityResult.h"

class CActor;
class CAbility;
class CAppliedEffectContainer;
struct SAnimation;

class CAbilityPerformer{
public:
    CAbilityPerformer(CActor *attached);
    ~CAbilityPerformer();
    
    enum EActorAbilityState {
        asIdle,
        asCasting,
        asDelay,
        asReadying
    };
protected:
    CActor *mAttached;
    CAppliedEffectContainer *mAppEffectContainer;
    int mAbilityAnim;
    EActorAbilityState mAbilityState;
    SAbilityInstance *mActiveAbility;
    int mActiveAbilityIndex;
    // flaga ustawiana, kiedy zmieni sie zawartosc wektora abilities;
    // potrzebne CPlayer
    bool mAbilityVectorChanged;

    float mAbilityStateDelayLeft;
    // efekt, ktory aktualnie stosujemy do aktora (jesli jakis jest)
    // w zwiazku z rzucaniem przez niego czaru
    EffectSourcePtr mySource;

    SAnimation *mReadyingAnim;
    
    // obiekt koncentracji :P
    FocusObjectPtr mFocusObject;
    CAbility *mFocusedAbility;
    float mFocusTime;
public:
	// wyciaga zapamietany kontekst dla umiejki lub tworzy nowy
	ExecutionContextPtr GetContext(int idx);

    inline int GetAbilityAnim(){ return mAbilityAnim; }

    inline EActorAbilityState GetAbilityState(){ return mAbilityState; }
    inline SAbilityInstance *GetActiveAbility(){ return mActiveAbility; }

    inline SAnimation* GetReadyingAnim(){ return mReadyingAnim; }
    void SetReadyingAnim(SAnimation *anim);

    // czyta flage mAbilityVectorChanged i ustawia ja na false
    inline bool IsAbilityVectorChanged(){
        bool tmp = mAbilityVectorChanged;
        mAbilityVectorChanged = false;
        return tmp;
    }
    // dodaje umiejetnosc, zwraca indeks
    int AddAbility(SAbilityInstance &abi, int anim = -1);
	// zwraca indeks umiejki w performerze
	int FindAbilityIndex(CAbility *abi);
    // todo:
    void AddOrSwapAbilityWithTrigger(SAbilityInstance &abi, const std::string & trigger, int anim = -1);
    // wykonuje umiejetnosc, niezaleznie czy jest dodana; nie wplywa na cooldown, tylko na delay
    EAbilityResult PerformAbility(SAbilityInstance &abi, bool ignoreMana = false, int data = -1,float extraCastingTime = 0.0f);
    // wykonuje umiejetnosc sposrod dodanych; dodatkowo mozna dodac juz wykorzystany casting time (wprowadzanie kombinacje)
    EAbilityResult PerformAbility(int i,float extraCastingTime = 0.0f);
    // wykonuje bazowy atak
    inline EAbilityResult DoDefaultAttack(){ return PerformAbility(mDefaultAttack); }
    // wybiera atak, ktory ma byc domyslny (dla potwora)
    inline void SetDefaultAttack(int i){ mDefaultAttack = i; }
    inline SAbilityInstance *GetAbility(unsigned int i) { 
        if ((i < 0) || (i >= mAbilities->size())) 
            return NULL;
        else
            return &(mAbilities->at(i));
    }
    inline int GetDefaultAttack(){ return mDefaultAttack; }
	void RandomizeDefaultAttack();
    inline int GetAbilityCount() { return (int)(mAbilities->size()); }

    inline std::vector<SAbilityInstance> *GetAbilities() { return mAbilities; }
	void AdvanceAbilityLevel(int id);

    // zwraca wektor indeksow dostepnych umiejetnosci (chodzi o cooldown)
    // potrzebne AI
    void GetAvailableAbilities(std::vector<int> &availableAbis);

    void Update(float dt);

    // koncentracja
    inline FocusObjectPtr GetFocusObject(){ return mFocusObject; }
    // zwraca, czy bylo co przerwac
    bool BreakFocus();
protected:
    // dostepne umiejetnosci
    std::vector<SAbilityInstance> *mAbilities;
    struct SAbilityData{
		float manacost;
		float cooldown;
		float delay;
		float casttime;

        float cooldownLeft;
        ExecutionContextPtr context;
		EffectSourcePtr source;

        SAbilityData(): manacost(0.f), cooldown(0.f), delay(0.f), casttime(0.f), cooldownLeft(0.0f), context(NULL), source(NULL) {}
    };
    std::vector<SAbilityData> mAbilityData;
    std::vector<int> *mAbilityAnims;
    // ktora umiejetnosc to standardowy atak?
    int mDefaultAttack;
public:
    // przyjmuje podane wektory za wlasne
    // uwaga! nie wykonaja sie umiejetnosci pasywne
    // uwaga! AddAbility itp. beda teraz operowac na zbindowanych wektorach
    inline void Bind(std::vector<SAbilityInstance> *abilities, std::vector<int> *anims){
        mAbilityAnims = anims;
        mAbilities =  abilities;
        if (abilities != NULL)
            mAbilityData.resize(abilities->size());
        else
            mAbilityData.resize(0);
    }

	bool GetActiveAbilityComputedValues(float &cooldown, float &delay, float &casttime);
};

#endif /* __CABILITYPERFORMER_H__ */

