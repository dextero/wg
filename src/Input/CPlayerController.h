#ifndef __CPLAYERCONTROLLER_H__
#define __CPLAYERCONTROLLER_H__

#include <vector>

#include "../Logic/CActorController.h"
#include "../Logic/Effects/CEffectSource.h"
#include "../Logic/Abilities/EAbilityResult.h"

#include <SFML/System/Vector2.hpp>

const unsigned int ABI_SLOTS_COUNT = 3;

class CPlayer;
class CAbility;
struct SAbilityInstance;
class CTriggerEffects;

class CInGameOptionChooser;
class CInteractionTooltip;

class CPlayerController : public CActorController
{
public:
    static const int AbilityKeyCount; // iloma klawiszami sie wybiera animacje
    static const float TurningTimeThreshold; // zeby dodac odrobine precyzji do obrotu
private:

	enum WalkingDir { wdNone = 0, wdForward = 1, wdBackward = 2, wdLeft = 4, wdRight =8};
	int mWalkingDir;

    enum TurningDir { tdNone, tdLeft, tdRight } mTurningDir;
    float mTurningTime; // calkowity czas utrzymywania kierunku obrotu    
    float mWalkingTime; // calkowity czas utrzymywania kierunku marszu
    
    int mLastKey;
    float mCastingTime;
    CAbility *mFocusAbility;
    bool mKeepFocus;
	CAbility *mLastAbility;
	EAbilityResult mLastResult;

    bool mControlsSwitched;

    CTriggerEffects *mTriggerEffects;
    EffectSourcePtr mySource;

    float mAbsoluteMoveX, mAbsoluteMoveY;
    bool mIsInAbsoluteMovement;
	bool mMouseLook;

    bool mHasWalkTarget;
    sf::Vector2f mWalkTarget;

    // sloty na czary
    int mSelectedAbilities[ABI_SLOTS_COUNT];

    CInGameOptionChooser * mOptionChooser;
    CInteractionTooltip * mInteractionTooltip;
public:
	CPlayerController(CPlayer *player);
	virtual ~CPlayerController();
    
    void SetTurning(bool right, bool left);
	void SetStrafing(bool right, bool left);
    void SetWalking(bool forward, bool back);	
	void SetRotation(float value);
    void SetWalkingAbsolute(bool north, bool east, bool south, bool west);
	void SetMouseLook(bool look);
    void SetWalkTarget(bool walk, sf::Vector2f target = sf::Vector2f(0.f, 0.f), bool force = false);

    void AbiKeyPressed(int idx, bool hold);
	void StartTalk();

    // wolane przez CPlayer
    virtual void Update(float dt);

    void SwitchControls();

    inline void BindTriggerEffects(CTriggerEffects *triggerEffects){ mTriggerEffects = triggerEffects; }

	inline EAbilityResult GetLastAbilityResult(){ return mLastResult; }
	inline CAbility *GetLastAbility(){ return mLastAbility; }
    inline int* GetSelectedAbilities() { return mSelectedAbilities; }
 
    bool AllowKeyHold();

    CInGameOptionChooser * GetOptionChooser();
    CInteractionTooltip * GetInteractionTooltip();
};

#endif /*__CPLAYERCONTROLLER_H__*/
