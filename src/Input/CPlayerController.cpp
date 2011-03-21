#include <cstdio>
#include "CPlayerController.h"
#include "../GUI/CHud.h"
#include "../GUI/CInGameOptionChooser.h"
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

const int CPlayerController::AbilityKeyCount = 4; // iloma klawiszami sie wybiera animacje
const float CPlayerController::TurningTimeThreshold = 0.15f; // zeby dodac odrobine precyzji do obrotu
const float CPlayerController::MaxSequenceIdleTime = 0.6f; // maksymalny czas pomiedzy klawiszami sekwencji
const float CPlayerController::SameKeyPause = 0.3f; // przerwa pomiedzy tymi samymi klawiszami sekwencji

class CAbilityKeyMap{
public:
    static const int ABILITY_NOT_FOUND = -1;
    static const int ABILITY_INCOMPLETE = -2;
private:
    struct Node{
        Node *children[CPlayerController::AbilityKeyCount];
        int abiIndex;

        Node(){
            for (int i = 0; i < CPlayerController::AbilityKeyCount; i++)
                children[i]=NULL;
            abiIndex = ABILITY_INCOMPLETE;
        }

        ~Node(){
            Clear();
        }

        void Clear(){
            for (int i = 0; i < CPlayerController::AbilityKeyCount; i++)
                if (children[i]!=NULL){
                    delete children[i];
                    children[i]=NULL;
                }
        }

        void Add(std::string &sequence,int seqIndex, int abiIndex){
            if (seqIndex >= (int)sequence.size()){
                this->abiIndex = abiIndex;
            } else {
                int c = StringUtils::CharToCode(sequence[seqIndex]);
                if (children[c] == NULL)
                    children[c] = new Node();
                children[c]->Add(sequence,seqIndex+1,abiIndex);
            }
        }

        int Find(std::vector<int> &sequence, int seqIndex){
            if (seqIndex >= (int)sequence.size())
                return abiIndex;
            int c = sequence[seqIndex];
            if (children[c] == NULL)
                return ABILITY_NOT_FOUND;
            return children[c]->Find(sequence,seqIndex+1);
        }
    } mRoot;

public:
    CAbilityKeyMap(){}
    ~CAbilityKeyMap(){}

    void Rebuild(std::vector<SAbilityInstance> &abilities){
        mRoot.Clear();
        for (int i = 0; i < (int)abilities.size(); i++){
			if (abilities[i].triggerEnabled && (abilities[i].ability->trigger.size() > 0))
                mRoot.Add(abilities[i].ability->trigger,0,i);
        }
    }

    int FindAbility(std::vector<int> &sequence){
        return mRoot.Find(sequence,0);
    }
};

CPlayerController::CPlayerController(CPlayer *player) :
    CActorController(player),
    currentActionType(KeyActionTypes::Null),
    mWalkingDir(wdNone),
    mTurningDir(tdNone),
    mTurningTime(0.f),
    mWalkingTime(0.f),
    mSequenceIdleTime(0.f),
    mLastKey(-1),
    mCastingTime(0.0f),
    mFocusAbility(NULL),
	mLastAbility(NULL),
    mLastResult(arUnknown),
	mAbilityActivatedJustNow(false),
    mControlsSwitched(false),
    mTriggerEffects(NULL),
    mAbsoluteMoveX(0.0f),
    mAbsoluteMoveY(0.0f),
    mIsInAbsoluteMovement(false),
	mMouseLook(false),
    mOptionChooser(NULL)
{
    fprintf(stderr,"CPlayerController::CPlayerController()\n");
    mKeyMap = new CAbilityKeyMap();
    mySource = new CEffectSource(estCastingAbility,NULL);

    // -1 == ABILITY_NOT_FOUND
    memset(mSelectedAbilities, -1, ABI_SLOTS_COUNT * sizeof(int));
}

CPlayerController::~CPlayerController(){
    fprintf(stderr,"CPlayerController::~CPlayerController()\n");
    if (mOptionChooser) {
        delete mOptionChooser;
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

void CPlayerController::AbiKeyPressed(KeyActionType actionType, int idx, bool hold){
    if (mFocusAbility){ // focusowe
        if (mLastKey != idx ){ // inny klawisz
            mActor->GetAbilityPerformer().BreakFocus();
            mFocusAbility = NULL;
        } else { // klawisz "podtrzymujacy"
            if (hold){
                mSequenceIdleTime = 0.0f;
            }
        }
    } else if ((!hold) || (mKeySequence.size() == 0)){ // nie-focusowe
        //if (mLastKey == idx)
        //    if (mSequenceIdleTime < SameKeyPause)
        //        return; // ot, taka blokada przeciw przytrzymaniu klawisza

		if (actionType== KeyActionTypes::OnlySlot) mKeySequence.clear();

		currentActionType = actionType;

        mKeySequence.push_back(idx);
        mLastKey = idx;
        mSequenceIdleTime = 0.0f;
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

void CPlayerController::RebuildAbilityData(std::vector<SAbilityInstance> &abilities){
    mKeyMap->Rebuild(abilities);
}

void CPlayerController::Update(float dt){

    if (mActor->GetSpawnState() != CActor::ssAlive) return;
    // reset stanu
    CActorController::Update(dt);

    // sterowanie 8-k, tox 6 sierpnia
    if ( mIsInAbsoluteMovement )
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
    if (mFocusAbility){
        if (mSequenceIdleTime > 0.0f){
            mFocusAbility = NULL;
            mActor->GetAbilityPerformer().BreakFocus();
        }
    }

	mAbilityActivatedJustNow = false;

    // umiejetnosci za pomoca sekwencji
    if (mSequenceIdleTime == 0.0f)
	{
		// dodano nowy klawisz
        
		int res = -1;

        // czy uzyc umiejek ze slotow?
        if (mKeySequence.size() == 1 && mKeySequence[0] >= 0 && mKeySequence[0] < (int)ABI_SLOTS_COUNT)
		{
			//fprintf(stderr, "slot!\n");

			if (currentActionType != KeyActionTypes::OnlyAbi)
			{
				res = mSelectedAbilities[mKeySequence[0]];
				//fprintf(stderr,"slot ok\n");
			}
		}
        else
		{
			//fprintf(stderr, "sekwencja!\n");

			if (currentActionType != KeyActionTypes::OnlySlot)
			{

				res = mKeyMap->FindAbility(mKeySequence);
				if (res == CAbilityKeyMap::ABILITY_NOT_FOUND)
				{
					mActor->GetAbilityPerformer().SetReadyingAnim(NULL);
					fprintf(stderr,"Sequence not found: ");
					for (int i = 0; i < (int)mKeySequence.size(); i++)
					{
						if (i > 0) fprintf(stderr,"-");
						fprintf(stderr,"%d",mKeySequence[i]);
					}
					fprintf(stderr,"\n");
					mKeySequence.clear();
				} 
				else 
				{
					if ((mKeySequence.size() > 0) && (mTriggerEffects != NULL))
					{
						CTriggerEffects::STriggerEffect *te = mTriggerEffects->Find(mKeySequence);
						if (te != NULL)
						{
							if (te->mSound != NULL)
								gAudioManager.GetSoundPlayer().Play( te->mSound );
							
							if (te->mEffect != NULL)
							{
								mActor->GetAppliedEffectContainer()->RemoveEffects(mySource);
								te->mEffect->Apply(mActor,mySource);
							}
							
							mActor->GetAbilityPerformer().SetReadyingAnim(te->mAnim);
						}
					}
				}
            }
        }
        if (res >= 0){
            mActor->GetAbilityPerformer().SetReadyingAnim(NULL);
			if (mKeySequence.size() == 1) mCastingTime = 0.0f;
            EAbilityResult result = mActor->GetAbilityPerformer().PerformAbility(res,mCastingTime);
            CAbility *abi = mActor->GetAbilityPerformer().GetAbilities()->at(res).ability;
			mLastAbility = abi;
            if (abi->isFocus){
                mFocusAbility = abi;
            }
            mCastingTime = 0.0f;
            if ((mKeySequence.size() > 1) || (result != arOK)){
                //fprintf(stderr,"Sequence: ");
                for (int i = 0; i < (int)mKeySequence.size(); i++){
                    if (i > 0) fprintf(stderr,"-");
                    //fprintf(stderr,"%d",mKeySequence[i]);
                }
                /*switch(result){
                    case arOK: fprintf(stderr," activated!\n"); break;
                    case arCooldown: fprintf(stderr," failed - cooldown\n"); break;
                    case arDelay: fprintf(stderr," failed - delay\n"); break;
                    case arNoMana: fprintf(stderr," failed - no mana\n"); break;
                    case arUnknown: fprintf(stderr," failed - unknown ability\n");
                    default: break;
                };*/
            }

			// przed wyczyszczeniem sekwencji trzeba zaktualizowac huda zeby zdazyl to wykryc
			if (gLogic.GetHud(0) != NULL) gLogic.GetHud(0)->Update(0.0f);
			if (gLogic.GetHud(1) != NULL) gLogic.GetHud(1)->Update(0.0f);
            if (result == arOK)
			{
				mAbilityActivatedJustNow = true;
                mLastKeySequence = mKeySequence;
			}
            mKeySequence.clear();
            mSequenceIdleTime = 0.0f;
			mLastResult = result;
        }
    }
    mSequenceIdleTime += dt;
    if (mKeySequence.size() > 0){
        mCastingTime+=dt;
        if (mSequenceIdleTime > MaxSequenceIdleTime){
            fprintf(stderr,"sequence time exceeded for sequence: ");
            mActor->GetAbilityPerformer().SetReadyingAnim(NULL);
            for (int i = 0; i < (int)mKeySequence.size(); i++){
                if (i > 0) fprintf(stderr,"-");
                fprintf(stderr,"%d",mKeySequence[i]);
            }
            fprintf(stderr,"\n");
            mKeySequence.clear();
            mSequenceIdleTime = 0.0f;
        }
    }
}

void CPlayerController::SwitchControls(){
    mControlsSwitched = !mControlsSwitched;
}

bool CPlayerController::AllowKeyHold(){
    return (((mKeySequence.size() == 0)&&(mLastKeySequence.size()==1)) || (mFocusAbility != NULL));
}

CInGameOptionChooser * CPlayerController::GetOptionChooser() {
    if (mOptionChooser == NULL) {
        fprintf(stderr, "Creating new chooser\n");
        mOptionChooser = CInGameOptionChooser::CreateChooser();
        mOptionChooser->SetPlayer((CPlayer *)mActor);
        mOptionChooser->SetOptionImages("data/GUI/btn-up.png", "data/GUI/btn-hover.png");
        mOptionChooser->SetOptionFont("data/GUI/verdana.ttf", 16.0f);
        mOptionChooser->SetOptionColor(sf::Color::White);
        mOptionChooser->SetOptionSize(sf::Vector2f(60.0f,60.0f));
        mOptionChooser->SetRadius(80.0f);
    }
    return mOptionChooser;
}


