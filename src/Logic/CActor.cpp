#include "CActor.h"
#include "CPhysicalManager.h"
#include "../Console/CConsole.h"
#include "../Utils/Maths.h"
#include "Effects/CEffectManager.h"
#include "Effects/CEffectHandle.h"
#include "Abilities/CAbility.h"
#include "CPlayerManager.h"
#include "CPlayer.h"
#include "Factory/CActorTemplate.h"
#include "Stats/CRPGCalc.h"
#include "CActorController.h"
#include "../Audio/CAudioManager.h"
#include "CLogic.h"

#include "../Rendering/CDisplayable.h"
#include "../Rendering/Animations/SAnimation.h"
#include "../Rendering/Animations/SAnimationState.h"
#include "../Rendering/Animations/CAnimSet.h"
#include "../Map/SceneManager/CSceneNode.h"
#include "../Rendering/ZIndexVals.h"

#include "../GUI/Messages/CMessageSystem.h"

#include "Conditions/CCondition.h"    

CActor::CActor(const std::wstring &uniqueId) :
CPhysical(uniqueId), 
mController(NULL),
mAnimationTime(0.0f), 
mAnimationTimeLeft(0.0f), 
mAnimSet(NULL), 
mXPValue(0.f),
mMoveState(msStanding), 
mSpawnState(ssAlive),
mCondition(NULL),
mStunLevel(0),
mHealthcastLevel(0),
mNextAbilityLevelBonus(0),
// zeby nie bylo - te thisy to zaden problem, przekazywany
// jest jedynie wskaznik i zapamietywany "na potem"
mStats(this),
mAppliedEffectContainer(this),
mBleeder(this),
mForceTime(0.0f),
mLastAnimation(NULL), 
mAbilityPerformer(this),
mCallbackDispatcher(this), 
mLair(NULL),
mSummonContainer(this)
 {
     mUseShadow = true;
     mSummonContainerPtr = &mSummonContainer;
}

CActor::~CActor() {
    mCallbackDispatcher.Dispatch();
	if (mController != NULL) delete mController;
}

void CActor::SetBoundingCircle( float radius )
{
	CPhysical::SetBoundingCircle( radius );
}

void CActor::UpdateAnimations(float dt){
    if (mAnimationTimeLeft != 0.0f){
        mAnimationTimeLeft -= dt;
        if (mSpawnState == ssDying){ // khe khe umieram...
            if (mAnimationTimeLeft < 0.0f)
                MarkForDelete();
            return;
        } else if (mAnimationTimeLeft < 0.0f){ // koniec delay
            mAnimationTimeLeft = 0.0f;
            if (mLastAnimation != NULL){
                SetAnimation(mAnimSet->GetDefaultAnim());
                mLastAnimation = NULL;
            }
        } else {
            if ((mAbilityPerformer.GetAbilityState() == CAbilityPerformer::asDelay) 
                || (mAbilityPerformer.GetAbilityState() == CAbilityPerformer::asCasting))
                if (mLastAnimation != NULL){
                    GetDisplayable()->GetAnimationState()->RewindTo(1.0f - 
                        mAnimationTimeLeft / mAnimationTime);
                }
        }
    } else {
        if (mSpawnState == ssDying){
            SetAnimation(mAnimSet->GetDeathAnim());
            mAnimationTimeLeft = mAnimationTime = GetDisplayable()->GetAnimationState()->animation->TotalLength();
            GetDisplayable()->GetAnimationState()->isLooped = false;
            GetDisplayable()->GetAnimationState()->RewindTo(0.0f);
            GetShadow()->GetAnimationState()->isLooped = false;
            GetShadow()->GetAnimationState()->RewindTo(0.0f);
        } else if (mAbilityPerformer.GetAbilityState() == CAbilityPerformer::asIdle){
            switch(mMoveState){
                case msStrafing:
                    if (mAnimSet->GetAnim(CAnimSet::acStrafe) != NULL){
                        SetAnimation(mAnimSet->GetAnim(CAnimSet::acStrafe));
                        break;
                    }// jak nie, to probujemy z animacja chodzenia
                case msTurning:
                case msWalking:
                    if (mAnimSet->GetMoveAnim() != NULL){
                        SetAnimation(mAnimSet->GetMoveAnim());
                        break;
                    } // jak nie, to dajemy domyslna
                case msStanding:
                    SetAnimation(mAnimSet->GetDefaultAnim()); // wspolna animacja dla ssSpawning, ssAlive, ssAsleep (mozna rozbudowac)
                    break;
            };
        } else if (mAbilityPerformer.GetAbilityState() == CAbilityPerformer::asReadying){
            SetAnimation(mAbilityPerformer.GetReadyingAnim());
        } else { // asDelay albo asCast
			float delay,cooldown,casttime;
			mAbilityPerformer.GetActiveAbilityComputedValues(cooldown,delay,casttime);
            mAnimationTime = delay + casttime;
            SAnimation *anim = NULL;
            if (mAbilityPerformer.GetAbilityAnim() != "")
                anim = mAnimSet->GetAnim(mAbilityPerformer.GetAbilityAnim());
            if (anim == NULL)
                anim = mAnimSet->GetAttackAnim();
            if (anim != NULL){
                SetAnimation(anim);
                mLastAnimation = anim;
                float animLength = GetDisplayable()->GetAnimationState()->animation->TotalLength();
                if ( mAnimationTime > animLength)
                    mAnimationTime = animLength;
            }
            mLastAnimation = anim;
            mAnimationTimeLeft = mAnimationTime;
        }
    }
}

void CActor::Update(float dt) {
    if ((mSpawnState == ssAlive) || (mSpawnState == ssAsleep)){
        if (mStunLevel > 0)
            mSpawnState = ssAsleep;
        else if (mForceTime*Maths::Length(mForce) > 0.5f)
            mSpawnState = ssAsleep;
        else if (mCondition != NULL) {
            if (!(mCondition->SpecificCheck()))
                mSpawnState = ssAsleep;
            else
                mSpawnState = ssAlive;
        } else
            mSpawnState = ssAlive;
    }
    UpdateAnimations(dt);
    mAppliedEffectContainer.Update(dt);
    mStats.RestoreMana(dt * mStats.GetCurrAspect(aManaRegen));
    // reaplikujemy efekty, ktore tego wymagaja
    // oraz czyscimy efekty, ktore sie skonczyly
    mAbilityPerformer.Update(dt);
    
    if ( mForceTime > 0.0f ) {
		// damork: clamp-uje, bo albo mialem za maly zasiego knockbacka albo mi szczury przez drzewa przelatywaly :)
		float force = Maths::Clamp( Maths::Length(mForce) * sqrtf(mForceTime), 0.00000001f, 10.0f );
		mVelocity += Maths::Normalize(mForce) * force;
        mForceTime -= 4*dt;
    }

    if (mSpawnState == ssDying)
        mStats.SetHP(0.0f);
	// ustawienie predkosci animacji:
	if(GetDisplayable() && GetDisplayable()->GetAnimationState())
	{
		float animSpeed = 1.0f/(GetDisplayable()->GetScale().x*GetDisplayable()->GetScale().x);
		GetDisplayable()->GetAnimationState()->animSpeed = animSpeed;
		if (GetShadow() && GetShadow()->GetAnimationState())
			GetShadow()->GetAnimationState()->animSpeed = animSpeed;
	}

    CPhysical::Update(dt);
}

void CActor::Kill(){
    if (mAnimSet->GetDeathAnim() == NULL){
        MarkForDelete();
        gPlayerManager.XPGained(mXPValue);
    } else if (mSpawnState != ssDying){
        SetZIndex(Z_DEAD_BODY);
        mSpawnState = ssDying;
        mAnimationTimeLeft = 0.0f;
        
        // dzwiek
        if (unsigned sndCount = mSound["death"].size())
		    gAudioManager.GetSoundPlayer().Play(mSound["death"][rand() % sndCount], mPosition);

        gPlayerManager.XPGained(mXPValue);
    }

    if (dynamic_cast<CPlayer*>(this) == NULL)
        gLogic.ScoreGained((unsigned)mXPValue);
}

void CActor::DoDamage(float dmg,DamageTypes::EDamageType dt,EffectSourcePtr source){
    if (mSpawnState != ssDying){
        dmg = gRPGCalc.CalculateDamageAfterResistance(dmg,dt,mStats.GetAllCurrAspects());

        float realDmg = dmg; // "prawdziwy" dmg, zadawany potworom, nie ten wyswietlany
        // zmniejsz obrazenia, jesli to nie gracz
        if (dynamic_cast<CPlayer*>(this) == NULL)
            realDmg /= gLogic.GetDifficultyFactor();
        else // jesli to gracz, to zwieksz
            dmg = realDmg *= gLogic.GetDifficultyFactor();

        mStats.DoDamage(realDmg);
        if ((source == NULL) || (source->primary.type != estAppliedEffect)){
            //source->PrintToStderr();
            mBleeder.Bleed(dmg,source);
        }else
            if(!mAppliedEffectContainer.TryCumulateUnbledDamage(realDmg,source->primary.appliedEffect))
                mBleeder.Bleed(dmg, source); // latajace cyferki maja byc wszedzie takie same
        // zmienia animset na drugi z kolei (zwykle _winged) gdy hp spadnie ponizej 50%
        // mozna rozwazyc zmiane tego mechanizmu... ale dzialac dziala :)
		CActorTemplate *templ = dynamic_cast<CActorTemplate*>(mTemplate);
        if ((mTemplate) && (templ->GetAvailableAnims().size() >= 2)
            && (mAnimSet == templ->GetAvailableAnims()[0])
            && (GetHP() * 2.0f < mStats.GetCurrAspect(aMaxHP))){
                SetAnimSet(templ->GetAvailableAnims()[1]);
        }
        if (mController)
            mController->GotDamageFrom(realDmg,source);
        if (source != NULL){
            bool brokenFocus = false;
//            if (source->primary.type != estAppliedEffect){
//                brokenFocus = mAbilityPerformer.BreakFocus();
//            } else {
//                if (source->primary.appliedEffect != NULL)
//                    if (!(source->primary.appliedEffect->brokenFocus)){
//                        brokenFocus = mAbilityPerformer.BreakFocus();
//                        source->primary.appliedEffect->brokenFocus = true;
//                    }
//            } 
            if (brokenFocus){
                gMessageSystem.AddMessageTo(this, sf::Color::Red, L"Czar przerwany!");
            }
        }
    }
}

void CActor::DoHeal(float heal)
{
    mStats.DoHeal(heal);
    gMessageSystem.AddMessageTof(this, sf::Color::Green, L"%.0f", heal);
}

void CActor::ApplyForce(sf::Vector2f force) { 
    mForce *= mForceTime;
    force = force / mStats.GetCurrAspect(aMass);
    mForce.x += force.x;
    mForce.y += force.y;
	mForceTime = Maths::Clamp(sqrtf(Maths::Length(mForce)), 0.85f, 1.2f); 
}

CStats *CActor::GetStats(){
    return &mStats;
}

CAppliedEffectContainer *CActor::GetAppliedEffectContainer(){
    return &mAppliedEffectContainer;
}

CBleeder *CActor::GetBleeder(){
    return &mBleeder;
}

void CActor::SetAnimSet(CAnimSet *as){
    if (mAnimSet){
        // plynna zmiana animacji
        SAnimation * currAnim = GetAnimation();
        const CAnimSet::NameAnimPairMap & oldAnims = mAnimSet->GetAnims();
        const CAnimSet::NameAnimPairMap & newAnims = as->GetAnims();
        std::string idx;
        for (CAnimSet::NameAnimPairMap::const_iterator i = oldAnims.begin() ; i != oldAnims.end() ; i++) {
            if (i->second.anim == currAnim) {
                idx = i->first;
                break;
            }
        }
        if (idx != "") {
            CAnimSet::NameAnimPairMap::const_iterator i = newAnims.find(idx);
            if (i != newAnims.end()) {
                if (i->second.anim != NULL) {
                    float currTime = GetDisplayable()->GetAnimationState()->currentTime;
                    SetAnimation(i->second.anim);
                    GetDisplayable()->GetAnimationState()->RewindTo(currTime);
                }
            }
        }
    }
    mAnimSet = as;
}

bool CActor::IsDead(){
    return GetSpawnState() == ssDying;
}

CPinnedAbilityBatch *CActor::GetPinnedAbilityBatch(){
	return NULL;
}
