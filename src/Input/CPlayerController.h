#ifndef __CPLAYERCONTROLLER_H__
#define __CPLAYERCONTROLLER_H__

#include <vector>

#include "../Logic/CActorController.h"
#include "../Logic/Effects/CEffectSource.h"
#include "../Logic/Abilities/EAbilityResult.h"

const unsigned int ABI_SLOTS_COUNT = 3;

class CPlayer;
class CAbility;
struct SAbilityInstance;
class CTriggerEffects;

class CAbilityKeyMap; // definicja w CPlayerController.cpp
class CInGameOptionChooser;

namespace KeyActionTypes
{
	enum e
	{
		Null,
		OnlyAbi,
		OnlySlot,
		Both
	};
};
typedef KeyActionTypes::e KeyActionType;

class CPlayerController : public CActorController
{
public:
    static const int AbilityKeyCount; // iloma klawiszami sie wybiera animacje
    static const float TurningTimeThreshold; // zeby dodac odrobine precyzji do obrotu
    static const float MaxSequenceIdleTime; // maksymalny czas pomiedzy klawiszami sekwencji
    static const float SameKeyPause; // przerwa pomiedzy tymi samymi klawiszami sekwencji
private:

	KeyActionType currentActionType;

	enum WalkingDir { wdNone = 0, wdForward = 1, wdBackward = 2, wdLeft = 4, wdRight =8};
	int mWalkingDir;

    enum TurningDir { tdNone, tdLeft, tdRight } mTurningDir;
    float mTurningTime; // calkowity czas utrzymywania kierunku obrotu    
    float mWalkingTime; // calkowity czas utrzymywania kierunku marszu
    
    CAbilityKeyMap *mKeyMap;
    std::vector<int> mKeySequence;
    std::vector<int> mLastKeySequence;
    float mSequenceIdleTime;
    int mLastKey;
    float mCastingTime;
    CAbility *mFocusAbility;
	CAbility *mLastAbility;
	EAbilityResult mLastResult;

	bool mAbilityActivatedJustNow; // wynosi true tylko przez jedna klatke -> mouse caster jest ukrywany
    bool mControlsSwitched;

    CTriggerEffects *mTriggerEffects;
    EffectSourcePtr mySource;

    float mAbsoluteMoveX, mAbsoluteMoveY;
    bool mIsInAbsoluteMovement;
	bool mMouseLook;

    // sloty na czary
    int mSelectedAbilities[ABI_SLOTS_COUNT];

    CInGameOptionChooser * mOptionChooser;
public:
	CPlayerController(CPlayer *player);
	virtual ~CPlayerController();
    
    void SetTurning(bool right, bool left);
	void SetStrafing(bool right, bool left);
    void SetWalking(bool forward, bool back);	
	void SetRotation(float value);
    void SetWalkingAbsolute(bool north, bool east, bool south, bool west);
	void SetMouseLook(bool look);

    void AbiKeyPressed(KeyActionType actionType, int idx, bool hold);
	void StartTalk();

    void RebuildAbilityData(std::vector<SAbilityInstance> &abilities);

    // wolane przez CPlayer
    virtual void Update(float dt);

    void SwitchControls();

    inline void BindTriggerEffects(CTriggerEffects *triggerEffects){ mTriggerEffects = triggerEffects; }

    inline const std::vector<int> &GetCurrentKeySequence(){ return mKeySequence; }
    inline const std::vector<int> &GetLastKeySequence(){ return mLastKeySequence; }
	inline EAbilityResult GetLastAbilityResult(){ return mLastResult; }
    inline float GetCurrentSequenceIdleTime(){ return mSequenceIdleTime; }
	inline CAbility *GetLastAbility(){ return mLastAbility; }
	inline bool AbilityActivatedJustNow(){ return mAbilityActivatedJustNow; }
    inline int* GetSelectedAbilities() { return mSelectedAbilities; }
 
    bool AllowKeyHold();

    CInGameOptionChooser * GetOptionChooser();
};

#endif /*__CPLAYERCONTROLLER_H__*/
