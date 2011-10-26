#include <cstdio>
#include "CPlayerController.h"
#include "../GUI/CHud.h"
#include "../GUI/CInGameOptionChooser.h"
#include "../GUI/CInteractionTooltip.h"
#include "../Logic/CLogic.h"
#include "../Logic/CPlayer.h"
#include "../Logic/CDetector.h"
#include "../Logic/CPhysicalManager.h"
#include "../Logic/Abilities/CAbility.h"
#include "../Logic/Abilities/SAbilityInstance.h"
#include "../Utils/StringUtils.h"

#include "../Audio/CAudioManager.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Logic/Abilities/CTriggerEffects.h"
#include "../Utils/Maths.h"
#include "CBindManager.h"
#include "../Logic/Items/CItem.h"

const float CPlayerController::TurningTimeThreshold = 0.45f; // zeby dodac odrobine precyzji do obrotu

CPlayerController::CPlayerController(CPlayer *player) :
    CActorController(player),
    mWalkingDir(wdNone),
    mTurningDir(tdNone),
    mTurningTime(0.f),
    mWalkingTime(0.f),
    mLastKey(-1),
    mCastingTime(0.0f),
    mFocusAbility(NULL),
    mKeepFocus(false),
	mLastAbility(NULL),
    mLastResult(arUnknown),
    mControlsSwitched(false),
    mTriggerEffects(NULL),
    mAbsoluteMoveX(0.0f),
    mAbsoluteMoveY(0.0f),
    mIsInAbsoluteMovement(false),
	mMouseLook(false),
    mHasWalkTarget(false),
    mOptionChooser(NULL),
    mInteractionTooltip(NULL)
{
    fprintf(stderr,"CPlayerController::CPlayerController()\n");
    mySource = new CEffectSource(estCastingAbility,NULL);

    // -1 == ABILITY_NOT_FOUND
    memset(mSelectedAbilities, -1, ABI_SLOTS_COUNT * sizeof(int));
}

CPlayerController::~CPlayerController(){
    fprintf(stderr,"CPlayerController::~CPlayerController()\n");
    if (mOptionChooser) {
        delete mOptionChooser;
    }
    if (mInteractionTooltip) {
        delete mInteractionTooltip;
    }
}

void CPlayerController::SetTurning(bool right, bool left){
    if ((right && left) || (!right && !left)){ // jesli oba albo zaden
        mTurningDir = tdNone;
        mTurningTime = 0.0f;
    } else {
        if (right)
            mTurningDir = tdRight;
        else
            mTurningDir = tdLeft;
    }
}

void CPlayerController::SetStrafing(bool right, bool left){
    if ((right && left) || (!right && !left)){ // jesli oba albo zaden
		mWalkingDir &= ~(wdLeft | wdRight);
    } else {
        if (right)
			mWalkingDir |= wdRight;
        else
            mWalkingDir |= wdLeft;
    }
}

void CPlayerController::SetWalking(bool forward, bool back){
    if ((back && forward) || (!forward && !back)){ // jesli oba albo zaden
        mWalkingDir &= ~(wdForward | wdBackward);
        mWalkingTime = 0.0f;
    } else {
        if (forward)
			mWalkingDir |= wdForward;
        else
            mWalkingDir |= wdBackward;
    }
}

void CPlayerController::SetWalkingAbsolute(bool north, bool east, bool south, bool west)
{
    //zaslepka
//    SetWalking( north, south );
//    SetTurning( east, west );

   float x, y;
   x = y = 0.0f;
   if ( north ) y -= 1.0;
   if ( south ) y += 1.0;
   if ( east ) x += 1.0;
   if ( west ) x -= 1.0;
   mAbsoluteMoveX = x;
   mAbsoluteMoveY = y;
   mIsInAbsoluteMovement = ( north || east || south || west );
}

void CPlayerController::SetMouseLook(bool look)
{
	mMouseLook = look;
}

void CPlayerController::SetWalkTarget(bool walk, sf::Vector2f target, bool force)
{
    // albo ma isc, albo jest w trakcie
    mHasWalkTarget = force ? walk : (walk || mHasWalkTarget);
    // bylo klikniecie = zmiana kierunku
    if (walk)
        mWalkTarget = target;
}

void CPlayerController::AbiKeyPressed(int idx, bool hold){
    if (mFocusAbility && mLastKey != idx) {
        mActor->GetAbilityPerformer().BreakFocus();
        mFocusAbility = NULL;
    }

    if (hold) {
        if (mFocusAbility && mLastKey == idx) {
            mKeepFocus = true;
        }
        mLastKey = idx;
    } else {
        mLastKey = -1;
    }
}

void CPlayerController::StartTalk()
{
	CDetector * npcChooser = gPhysicalManager.CreateDetector();
	npcChooser->SetPosition( mActor->GetPosition() );
	npcChooser->SetBoundingCircle( 2.0f );
	npcChooser->SetCollidingPhysicals( PHYSICAL_NPC );
	npcChooser->SetBehaviour( CHOOSE_NPC_TO_TALK );
	npcChooser->SetFramesToDeath( 1 );
}

void CPlayerController::Update(float dt) {
    if (mActor->GetSpawnState() != CActor::ssAlive) return;
    // reset stanu
    CActorController::Update(dt);

    // podazanie do celu wskazanego kliknieciem mysza, #1093 by dex
    if (mHasWalkTarget)
    {
        if (Maths::LengthSQ(mActor->GetPosition() - mWalkTarget) > 0.1f)
        {
            float speed = mActor->GetStats()->GetCurrAspect(aSpeed);
            SetMove(1);
            SetSpeed(speed);
            mActor->SetVelocity(Maths::Normalize(mWalkTarget - mActor->GetPosition()) * speed);
        }
        else
            mHasWalkTarget = false;
    }
    // sterowanie 8-k, tox 6 sierpnia
    else if ( mIsInAbsoluteMovement )
    {
        sf::Vector2f move( Maths::Normalize( sf::Vector2f( mAbsoluteMoveX, mAbsoluteMoveY ) ) );

        float desiredRot = Maths::AngleBetween( move, Maths::VectorUp() );
        float speed = mActor->GetStats()->GetCurrAspect(aSpeed);
        SetMove(1);
        SetSpeed( speed );
        mActor->SetVelocity( move * speed );
		if (!mMouseLook)
			TurnBy( desiredRot - mTrueRot );
    }
    else
    {
        // lewo - prawo (stafe)
    	if (mWalkingDir & wdRight)
    		SetStrafe(1);
    	else if (mWalkingDir & wdLeft)
    	    SetStrafe(-1);

        // tyl-przod
        if (mWalkingDir & wdForward)
    		SetMove(1);
        else if (mWalkingDir & wdBackward)
            SetMove(-1);

        SetSpeed(mActor->GetStats()->GetCurrAspect(aSpeed));
        mWalkingTime+=dt;

        // obroty
        float multiplier = 0.0f;
        if (mTurningDir == tdLeft){
            if ((mWalkingDir & wdBackward) && mControlsSwitched){
                multiplier=-1.0f;
            } else
                multiplier=1.0f;
        }else if (mTurningDir == tdRight) {
            if ((mWalkingDir & wdBackward) && mControlsSwitched){
                multiplier=1.0f;
            } else
                multiplier=-1.0f;
        }
        if (mTurningTime < TurningTimeThreshold)
            multiplier*=(1.0f-(TurningTimeThreshold - mTurningTime)/TurningTimeThreshold);
        mTurningTime+=dt;
        float value = mActor->GetMaxTurnSpeed()*dt*multiplier;

		if (!mMouseLook)
			TurnBy(value);
    }

    // focus
    if (mFocusAbility) {
        if (!mKeepFocus) {
            mFocusAbility = NULL;
            mActor->GetAbilityPerformer().BreakFocus();
        }
    }

    if (mLastKey != -1 && !mKeepFocus)
	{
		int res = -1;

        CItem * item = ((CPlayer*)mActor)->GetItem(mLastKey);
        // dex: nie wiem jak to napisac lepiej, zeby nie smiecic niepotrzebnymi flagami
        if (mLastKey == 0 && ((CPlayer*)mActor)->GetCurrentItem())
            item = ((CPlayer*)mActor)->GetCurrentItem();

        if (item) {
            res = mActor->GetAbilityPerformer().FindAbilityIndexByInvPos(item->mInvPos);
        }

        if (res >= 0){
            mActor->GetAbilityPerformer().SetReadyingAnim(NULL);
			mCastingTime = 0.0f;
            EAbilityResult result = mActor->GetAbilityPerformer().PerformAbility(res,mCastingTime);
            CAbility *abi = mActor->GetAbilityPerformer().GetAbilities()->at(res).ability;
			mLastAbility = abi;
            if (abi->isFocus){
                mFocusAbility = abi;
            }
            mCastingTime = 0.0f;
            /* if (result != arOK) {
                switch(result){
                    case arOK: fprintf(stderr," activated!\n"); break;
                    case arCooldown: fprintf(stderr," failed - cooldown\n"); break;
                    case arDelay: fprintf(stderr," failed - delay\n"); break;
                    case arNoMana: fprintf(stderr," failed - no mana\n"); break;
                    case arUnknown: fprintf(stderr," failed - unknown ability\n");
                    default: break;
                };
            } */

			// przed wyczyszczeniem sekwencji trzeba zaktualizowac huda zeby zdazyl to wykryc
			if (gLogic.GetHud(0) != NULL) gLogic.GetHud(0)->Update(0.0f);
			if (gLogic.GetHud(1) != NULL) gLogic.GetHud(1)->Update(0.0f);
			mLastResult = result;
        }
    }
    mKeepFocus = false;
}

void CPlayerController::SwitchControls(){
    mControlsSwitched = !mControlsSwitched;
}

bool CPlayerController::AllowKeyHold(){
    return (mLastKey != -1 && mFocusAbility != NULL);
}

CInGameOptionChooser * CPlayerController::GetOptionChooser() {
    if (mOptionChooser == NULL) {
        fprintf(stderr, "Creating new chooser\n");
        mOptionChooser = CInGameOptionChooser::CreateChooser();
        mOptionChooser->SetPlayer((CPlayer *)mActor);
        mOptionChooser->SetOptionImages("data/GUI/btn-up.png", "data/GUI/btn-hover.png");
        mOptionChooser->SetOptionFont(gGUI.GetFontSetting("FONT_DEFAULT"));
        mOptionChooser->SetOptionColor(sf::Color::White);
        mOptionChooser->SetOptionSize(sf::Vector2f(60.0f,60.0f));
        mOptionChooser->SetRadius(80.0f);
    }
    return mOptionChooser;
}

CInteractionTooltip * CPlayerController::GetInteractionTooltip() {
    if (mInteractionTooltip == NULL) {
        fprintf(stderr, "Creating a new interactionTooltip\n");
        mInteractionTooltip = new CInteractionTooltip();
    }
    return mInteractionTooltip;
}
