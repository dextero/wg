#include "CAbilityPerformer.h"
#include "CAbility.h"
#include "../CActor.h"
#include "../Effects/CEffectHandle.h"
#include "../Effects/CEffectManager.h"
#include "../Effects/CExecutionContext.h"
#include "../../Utils/CRand.h"
#include <cassert>

#include "../CPlayer.h"

CAbilityPerformer::CAbilityPerformer(CActor *attached) :
    mAttached(attached),
    mAbilityAnim(""),
    mAbilityState(asIdle),
    mActiveAbility(NULL),
    mActiveAbilityIndex(0),
    mAbilityVectorChanged(false),
	mAbilityStateDelayLeft(0.0f),
    mySource(NULL),
    mReadyingAnim(NULL),
    mFocusObject(NULL),
    mFocusedAbility(NULL),
    mFocusTime(0.0f),
    mDefaultAttack(0)
{
    mAppEffectContainer = mAttached->GetAppliedEffectContainer();
}

CAbilityPerformer::~CAbilityPerformer(){}

int CAbilityPerformer::AddAbility(SAbilityInstance &abi, const std::string & anim, size_t invPos){
    if (!abi.ability) return -1;
    mAbilities->push_back(abi);
    SAbilityData ad;
    ad.invPos = invPos;
    ad.context = CExecutionContext::Alloc(mAttached->GetSafePointer(),mAttached,&abi,mAttached->GetPinnedAbilityBatch());
	ad.source = new CEffectSource(abi.ability);
    ad.source->SetSecondary(mAttached);
    mAbilityData.push_back(ad);
    mAbilityAnims->push_back(anim);
    if (abi.ability->abilityType == atPassive)
		gEffectManager.ApplyPermanent(mAttached,abi.ability->effect->GetOffset(),ad.source,ad.context);
    mAbilityVectorChanged = true;

    // odswiez wskaznik na mActiveAbility (gdyby wektor sie przesunal)
    if (mActiveAbility != NULL)
        mActiveAbility = &mAbilities->at(mActiveAbilityIndex);

    return (int)(mAbilities->size()-1);
}

int CAbilityPerformer::FindAbilityIndex(CAbility *abi) {
	for (unsigned int i = 0; i < mAbilities->size(); i++) {
		if (mAbilities->at(i).ability == abi)
			return i;
	}
	return -1;
}

int CAbilityPerformer::FindAbilityIndexByInvPos(size_t invPos) {
    size_t i = mAbilityData.size();
    while (i > 0) {
        // dex: !@#$%^AAAA "ja sobie tu zapomne -1 przy i, a wy sobie debugujcie, czemu umiejka nie dziala". dzieki, tox :D
        if (mAbilityData[i-1].invPos == invPos) { // przegladamy od tylu, bo nowo dodawane abilities beda ladowac na koncu listy i 'przykrywac' poprzednie
            return i-1;                           // ... do czasu az ktos dopisze usuwanie umiejetnosci :P
        }
        i--;
    }
    return -1;
}

void CAbilityPerformer::AdvanceAbilityLevel(int id){
	mAbilities->at(id).level++;
	CAbility *abi = mAbilities->at(id).ability;
	SAbilityData &ad = mAbilityData[id];
	mAbilityData[id].context->values.Set(aLevel,(float)(mAbilities->at(id).level));
	if (abi->abilityType == atPassive){
		CAppliedEffectContainer *aec = mAttached->GetAppliedEffectContainer();
		if (aec!=NULL)
			aec->RemoveEffects(ad.source);
        gEffectManager.ApplyPermanent(mAttached,abi->effect->GetOffset(),ad.source,ad.context);
	}
}

ExecutionContextPtr CAbilityPerformer::GetContext(int idx){
	ExecutionContextPtr context;
    if (idx >= 0){
        context = mAbilityData[idx].context;
		SAbilityInstance &abi = mAbilities->at(idx);
		if ((context == NULL) || !(context->StillValidFor(mAttached,&abi)) || (mAttached->ReadNextAbilityLevel() != 0)){
            context = CExecutionContext::Alloc((mAttached ? mAttached->GetSafePointer() : Memory::CSafePtr<CActor>::NullPtr()),mAttached,&abi,mAttached->GetPinnedAbilityBatch());  //CHECK
            mAbilityData[idx].context = context;
        }
    } else {
        context = CExecutionContext::Alloc((mAttached ? mAttached->GetSafePointer() : Memory::CSafePtr<CActor>::NullPtr()),mAttached,(SAbilityInstance*)NULL,mAttached->GetPinnedAbilityBatch());  //CHECK
    }
	return context;
}

EAbilityResult CAbilityPerformer::PerformAbility(SAbilityInstance &abi, bool ignoreMana, int data,float extraCastingTime){
    BreakFocus(); // ot, na wszelki wypadek
    if (abi.ability==0) return arUnknown;
    if (mAbilityStateDelayLeft > 0.0f) return arDelay;
    CStats *stats = mAttached->GetStats();
    ExecutionContextPtr context;
    /*CActor* source = 0; // TODO: WTF, czemu nie poprostu mAttached ?
	// testowo przelacze na mAttached...
    if ((size_t)mActiveAbilityIndex < mAbilityData.size())
    {
        if (mAbilityData[mActiveAbilityIndex].source.operator ->())
            source = dynamic_cast<CActor*>(mAbilityData[mActiveAbilityIndex].source->DeterminePhysicalSource());
    }*/
	context = GetContext(data);
	context->abilityPower = abi.ability->mPower.Evaluate(context);
	float manaCost = abi.ability->mManaCost.Evaluate(context);
	float casttime = abi.ability->casttime.Evaluate(context);
	float delay = abi.ability->delay.Evaluate(context);
	float cooldown = abi.ability->cooldown.Evaluate(context);
    if (!ignoreMana && (manaCost != 0.0f)){
        if (stats == NULL){
            fprintf(stderr,"WARNING while performing ability %ls: tried to drain mana on object not having any. aborting\n", abi.ability->name.c_str());
            return arNoMana;
        }
		if (!(mAttached->GetHealthcast())){ // moze rzucamy za pomoca zdrowia...
			if (!(stats->TryDrainMana(manaCost)))
                if (stats->GetCurrAspect(aMaxMana) == stats->GetMana()) // jesli jest max many, to wszystko ok, mozna rzucac 
                    stats->SetMana(stats->GetMana() - manaCost);
                else
				    return arNoMana;
		} else {
			if (!(stats->TryDrainHealth(manaCost)))
				return arNoHealth;
		}
    }
    mActiveAbilityIndex = data;
    if (abi.ability->isFocus){// umiejka focusowa
        mFocusObject = CFocusObject::Alloc();
        mFocusedAbility = abi.ability;
        mFocusTime = abi.ability->focusMaxDuration.Evaluate(context);
    }
    if (casttime < extraCastingTime){
        abi.ability->Perform(mAttached,context,mFocusObject);
        mActiveAbility = &abi;
        mAbilityState = asDelay;
        if (abi.ability->mDelayEffect != NULL){
            mySource = new CEffectSource(estDelayingAbility,abi.ability);
            abi.ability->mDelayEffect->ApplyPermanent(mAttached,mySource);
        }
    }else{
        mActiveAbility = &abi;
        mAbilityState = asCasting;
        if (abi.ability->mCastingEffect != NULL){
            mySource = new CEffectSource(estCastingAbility,abi.ability);
            abi.ability->mCastingEffect->ApplyPermanent(mAttached,mySource,context);
        }
    }
    mAbilityStateDelayLeft = casttime + delay - extraCastingTime;
	//if (dynamic_cast<CPlayer*>(mAttached) != NULL)
	//	fprintf(stderr,"CAP::PerformAbility: setting mAbilityStateDelayLeft to %f\n", mAbilityStateDelayLeft);
	if (mActiveAbilityIndex >= 0){
		mAbilityData[mActiveAbilityIndex].manacost = manaCost;
		mAbilityData[mActiveAbilityIndex].casttime = casttime;
		mAbilityData[mActiveAbilityIndex].delay = delay;
		mAbilityData[mActiveAbilityIndex].cooldown = cooldown;
	}
    return arOK;
}

EAbilityResult CAbilityPerformer::PerformAbility(int i, float extraCastingTime){
    if ((i<0)||(((unsigned int)i)>=mAbilities->size()))
        return arUnknown;
    SAbilityInstance &abi = mAbilities->at(i);
    if (abi.ability==0) return arUnknown;
    if (mAbilityData[i].cooldownLeft > 0.0f) return arCooldown;
    EAbilityResult result = PerformAbility(abi,false,i,extraCastingTime);
    if (result == arOK){
        mAbilityData[i].cooldownLeft = mAbilityData[i].cooldown + mAbilityData[i].delay;
        mAbilityAnim = mAbilityAnims->at(i);
    }
    return result;
}

void CAbilityPerformer::Update(float dt){
    if (mFocusObject != NULL){
        assert(mFocusedAbility);
        CStats *stats = mAttached->GetStats();
        if (stats != NULL){
            float manaDrain = dt * mFocusedAbility->focusManaCost.Evaluate(mAbilityData[mActiveAbilityIndex].context);
            if (!(stats->TryDrainMana(manaDrain))) {
                BreakFocus();
            }
        }
        mFocusTime -= dt;
        if (mFocusTime < 0.0f)
            BreakFocus();
    }
	//if (dynamic_cast<CPlayer*>(mAttached) != NULL)
	//	fprintf(stderr,"CAP::Update: mAbilityStateDelayLeft is %f, dt is %f\n", mAbilityStateDelayLeft,dt);
    // uaktualniamy casttime/delay
    if (mAbilityStateDelayLeft != 0.0f){
        mAbilityStateDelayLeft -= dt;
        if (mAbilityStateDelayLeft < 0.0f){ // koniec delay
            if (mAbilityState == asCasting){
                ExecutionContextPtr context = mAbilityData[mActiveAbilityIndex].context;
                if ((context == NULL) || !(context->StillValidFor(mAttached,mActiveAbility))){
                    CActor* source = 0;
                    if ((size_t)mActiveAbilityIndex < mAbilityData.size())
                    {
                        if (mAbilityData[mActiveAbilityIndex].source.operator ->())
                            source = dynamic_cast<CActor*>(mAbilityData[mActiveAbilityIndex].source->DeterminePhysicalSource());
                    }
                    context = CExecutionContext::Alloc((source ? source->GetSafePointer() : Memory::CSafePtr<CActor>::NullPtr()),mAttached,mActiveAbility,mAttached->GetPinnedAbilityBatch()); //CHECK
                    mAbilityData[mActiveAbilityIndex].context = context;
                }
                mActiveAbility->ability->Perform(mAttached,context,mFocusObject);
                if (mActiveAbility->ability->mCastingEffect != NULL){
                    if (mAppEffectContainer != NULL)
                        mAppEffectContainer->RemoveEffects(mySource);
                    mySource = NULL;
                }
                mActiveAbility = NULL;
            } else if (mAbilityState == asDelay){
                if (mActiveAbility->ability->mDelayEffect != NULL){
                    if (mAppEffectContainer != NULL)
                        mAppEffectContainer->RemoveEffects(mySource);
                    mySource = NULL;
                }
            }
            mAbilityState = asIdle;
            mAbilityStateDelayLeft = 0.0f;
			//if (dynamic_cast<CPlayer*>(mAttached) != NULL)
			//	fprintf(stderr,"CAP::Update: mAbilityStateDelayLeft set to 0\n");
        } else { // koniec casting
			if ((mAbilityState == asCasting) && (mAbilityStateDelayLeft < mAbilityData[mActiveAbilityIndex].delay)){
                if (mActiveAbility->ability->mCastingEffect != NULL){
                    if (mAppEffectContainer != NULL)
                        mAppEffectContainer->RemoveEffects(mySource);
                }
                if (mActiveAbility->ability->mDelayEffect != NULL) {
                     mySource = new CEffectSource(estDelayingAbility,mActiveAbility->ability);
                     mActiveAbility->ability->mDelayEffect->ApplyPermanent(mAttached,mySource);
                }
                mAbilityState = asDelay;
                ExecutionContextPtr context = NULL;
                if (mActiveAbilityIndex >= 0)
                    context = mAbilityData[mActiveAbilityIndex].context;
                if ((mActiveAbility->ability->isFocus) && (mFocusTime <= 0.0)) {
                    // nic nie rob, gracz puscil klawisz zanim uplynal cast-time
                } else {
                    mActiveAbility->ability->Perform(mAttached,context,mFocusObject);
                }
            }
        }
    }
    // zmniejszamy cooldown
    for (unsigned int i = 0; i < mAbilityData.size(); i++){
        mAbilityData[i].cooldownLeft-=dt;
        if (mAbilityData[i].cooldownLeft < 0.0f)
            mAbilityData[i].cooldownLeft = 0.0f;
    }
}

void CAbilityPerformer::SetReadyingAnim(SAnimation *anim){
    if ((mAbilityState == asIdle) || (mAbilityState == asReadying)){
        if (anim == NULL){
            mAbilityState = asIdle;
            mReadyingAnim = NULL;
        }else{
            mAbilityState = asReadying;
            mReadyingAnim = anim;
        }
    }
}

bool CAbilityPerformer::GetActiveAbilityComputedValues(float &cooldown, float &delay, float &casttime){
	if ((mActiveAbility==NULL)||(mActiveAbilityIndex < 0))
		return false;
	else {
		cooldown = mAbilityData[mActiveAbilityIndex].cooldown;
		delay = mAbilityData[mActiveAbilityIndex].delay;
		casttime = mAbilityData[mActiveAbilityIndex].casttime;
		return true;
	}
}

void CAbilityPerformer::GetAvailableAbilities(std::vector<int> &availableAbis){
    for (unsigned int i = 0; i < mAbilityData.size(); i++)
        if (mAbilityData[i].cooldownLeft == 0.0f)
            availableAbis.push_back(i);
}

bool CAbilityPerformer::BreakFocus(){
    if (mFocusObject != NULL){
        mFocusObject->Invalidate();
        mFocusObject = NULL;
        mFocusedAbility = NULL;
        mFocusTime = 0.0f;
        fprintf(stderr,"Focus broken!\n");
        return true;
    } else {
        return false;
    }
}

void CAbilityPerformer::RandomizeDefaultAttack(){
	std::vector<int> candidates;
	for (unsigned int i = 0; i < mAbilities->size(); i++){
		EAbilityType at = mAbilities->at(i).ability->abilityType;
		if ((at == atBullet) || (at == atSelf) || (at == atMelee))
			candidates.push_back(i);
	}
	if (candidates.size() == 0) return;
	int r = gRand.Rnd(0,candidates.size());
	mDefaultAttack = candidates[r];
	fprintf(stderr,"Ability set to %s\n",StringUtils::ConvertToString(mAbilities->at(mDefaultAttack).ability->name).c_str());
}
