#ifndef __CABILITY_H__
#define __CABILITY_H__

#include <string>

#include "../../ResourceManager/IResource.h"
#include "../Effects/CExecutionContext.h"
#include "../Effects/CComputedValue.h"
#include "../Effects/CFocusObject.h"

class CActor;
class CPhysical;
class CEffectHandle;
class CAIAbilityHint;

enum EAbilityType {
    atNone = 0,
    atMelee = 1,
    atBullet = 2,
    atSelf = 3,
    atPassive = 4,
	atExport
};

class CBulletEmitter;
struct SAnimation;

class CAbility: public IResource{
protected:
    void PerformMelee(CActor *actor, ExecutionContextPtr context = ExecutionContextPtr(NULL), FocusObjectPtr focus = FocusObjectPtr(NULL));

    virtual bool Load(std::map<std::string,std::string>& argv);

    std::string effectDescription;
    std::map<std::string, std::string> effectDescriptionParameters;
public:
    std::wstring name;
    std::wstring description;
    std::wstring runesDescription;
    std::string trigger;
    std::string icon;
    std::string unavailableIcon;
    CAbility();

    virtual void Drop();

	virtual std::string GetType();

    CComputedValue mManaCost;

    // czestosc/szybkosc rzucania
    CComputedValue cooldown;
    CComputedValue delay;
    CComputedValue casttime;

    EAbilityType abilityType;
    CEffectHandle *effect;
    CEffectHandle *selfEffect;
    void Perform(CActor *actor, ExecutionContextPtr context = ExecutionContextPtr(NULL), FocusObjectPtr focus = FocusObjectPtr(NULL));

    // efekty aplikowane do aktora wykonujacego umiejetnosc, aplikowane
    // na czas trwania rzucania i/lub delay po efekcie
    // dodawane (czas trwania +INF) i usuwane recznie przez CAbilityPerformer
    CEffectHandle *mCastingEffect;
    CEffectHandle *mDelayEffect;

    // ataki pociskami
    CBulletEmitter *mBulletEmitter;
    
    // pola wspolne
    CComputedValue range;

    // ataki wrecz
    float angularWidth;
    bool InMeleeRange(CPhysical *attacker, CPhysical *victim, ExecutionContextPtr context);

    // umiejetnosci focusowe
    bool isFocus;
    CComputedValue focusManaCost;
    CComputedValue focusMaxDuration;

    CAIAbilityHint *mAIHint;

    // maksymalny poziom umiejki
    int mMaxLevel;

    // czy umiejke da sie kupic?
    bool mCanBuy;

    // szacowana "sila" umiejki
    CComputedValue mPower;

    // wyciag z <power calc=".." /> opis, jak bardzo umiejka zalezy od run
    void ParseRunesDescription(const std::string& str);

    // opis czaru, z uwzglednieniem wyliczanych parametrow
    const std::string GetEffectDescription(CActor* performer);
    const std::string GetManaCostString(CActor* performer);
};

#endif

