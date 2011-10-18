#ifndef CPLAYER_H_
#define CPLAYER_H_

#include "CActor.h"
#include "../Utils/Directions.h"

class CItem;
class CPlayerController;
class CAbilityTree;

enum EAbilityBuyingResult{
    abrOK,
    abrNoPoints,
    abrLevelReqNotMet,
    abrMaxLevelReached,
	abrTooLowPreqLevel,
    abrNoBuy
};

enum EHudDesc {
    HUDDESC_CIRCLE1 = 0,
    HUDDESC_BG      = 1,
    HUDDESC_AVATAR0 = 2,
    HUDDESC_AVATAR1 = 3,
    HUDDESC_AVATAR2 = 4,
    HUDDESC_AVATAR3 = 5,
    HUDDESC_AVATAR4 = 6,
    HUDDESC_CIRCLE2 = 7,
    HUDDESC_CIRCLE3 = 8,
    HUDDESC_HP      = 9,
    HUDDESC_MANA    = 10,
    HUDDESC_MANA_FLARED = 11,
    HUDDESC_XP      = 12,
    HUDDESC_FG      = 13,
    HUDDESC_FG_FLARED   = 14,
    HUDDESC_SEQ_Q   = 15,
    HUDDESC_SEQ_W   = 16,
    HUDDESC_SEQ_E   = 17,
    HUDDESC_SEQ_R   = 18,

    HUDDESC_COUNT   = 19
};

struct SHudDesc {
    std::string path[HUDDESC_COUNT];
};

class CPlayer : public CActor
{
    friend class CPhysicalManager;
protected:
	CPlayer(const std::wstring& uniqueId);
	virtual ~CPlayer();

	int mNumber; // nr gracza
    float mTotalXP;
    bool mImmortal;
    bool mDied;
    int mGold;

    int mSkillPoints, mAttrPoints;

    std::vector<CItem*> mItems; 
    int mCurrentItem;
    std::vector<CAbilityTree*> *mAbilityTrees;
    std::vector<std::string> *mAbiCodes;

	CPinnedAbilityBatch *mPinned;

    SHudDesc mHudDesc;
public:
    inline int GetNumber()				{ return mNumber; }
    inline void SetNumber(int number)	{ mNumber = number; }

    virtual void Update(float dt);
    virtual void Kill();

    inline CPlayerController *GetController() { return (CPlayerController*)mController; }

    void NextLevel( bool ignoreSkillPoints = false, bool muteSound = false );
    void XPGained(float xp, bool ignoreSkillPoints = false, bool muteSound = false);
    float XPRequired();
    float XPPreviouslyRequired();
	inline float GetTotalXP(){ return mTotalXP; }
    int GetLevel();
	inline int GetSkillPoints() { return mSkillPoints; }

    inline void SetGold(int newValue) { mGold = newValue; }
    inline int GetGold() { return mGold; }

    CItem * GetItem(size_t invPos);
    void AddItem(CItem * item, size_t invPos);
    void RemoveItem(CItem * item);

    CItem* GetCurrentItem();
    void SwitchCurrentItem(bool reverse = false);
    void InvalidateCurrentItem() { mCurrentItem = 0; }  // resetowanie, taki hack zeby Abi0 po zmianie sterowania strzelalo slotem 0

private:
    void AddAbilityFromTree(const SAbilityNode *an);
    bool CheckPreqs(int treeId, const std::vector<std::pair<int, int> > &preqs);
public:
    inline std::vector<CAbilityTree*> *GetBoundAbilityTrees(){ return mAbilityTrees; }
    void BindAbilityTrees(std::vector<CAbilityTree*> *mAbiTrees, std::vector<std::string> *mAbiCodes);

	std::vector<std::string> *GetAbiCodes(){ return mAbiCodes; }

    // kupuje poziom umiejetnosci, bez patrzenia na punkty, wymagania itp
    void AdvanceAbilityLevel(int treeId, int abiId);
    // zwraca, czy sa spelnione warunki na zakup
    EAbilityBuyingResult CanBuyAbilityLevel(int treeId, int abiId);
    // "normalny" zakup umiejetnosci
    EAbilityBuyingResult TryBuyAbilityLevel(int treeId, int abiId);
	// wlacz trigger w podanej umiejetnosci i jednoczesnie wylacz we wszystkich, które sa odpalane taka sama kombinacja
	void EnableTriggerInAbility(int treeId, int abiId);
    // funkcje wyszukujace indeksy na podstawie kodu
    int FindAbilityTree(const std::string &code);
    int FindAbilityInTree(int treeId, const std::string &code);

    void SetGodMode(bool gm);
    inline void AddSkillPoints(int amount){ mSkillPoints += amount; }

	std::wstring GetUnboughtPrequisitivesString(int treeIdx, int abiIdx);

	virtual CPinnedAbilityBatch *GetPinnedAbilityBatch();

    void SetHudDesc(SHudDesc desc) { mHudDesc = desc; };
    SHudDesc& GetHudDesc() { return mHudDesc; };

    // obliczanie sily umiejetnosci
    float GetAbilityPower(CAbility* abi);
    // sila umiejetnosci po ulepszeniu innej
    float GetAbilityPowerAtNextLevelOfOther(CAbility* abi, CAbility* toUpgrade);
    // wyciaganie wszystkich umiejek, ktore maja wplyw na sile innych
    std::vector<CAbility*> GetExportedAbilities();
};

#endif /*CPLAYER_H_*/
