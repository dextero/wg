#ifndef CPLAYER_H_
#define CPLAYER_H_

#include "CActor.h"
#include "../Utils/Directions.h"
#include "../GUI/Localization/CLocale.h"
#include <SFML/Graphics/Rect.hpp>

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
    HUDDESC_BG,
    HUDDESC_AVATAR0,
    HUDDESC_AVATAR1,
    HUDDESC_AVATAR2,
    HUDDESC_AVATAR3,
    HUDDESC_AVATAR4,
    HUDDESC_CIRCLE2,
    HUDDESC_CIRCLE3,
    HUDDESC_HP,
	HUDDESC_HP_BG,
    HUDDESC_MANA,
	HUDDESC_MANA_BG,
    HUDDESC_MANA_FLARED,
    HUDDESC_XP,
	HUDDESC_XP_BG,
    HUDDESC_FG,
    HUDDESC_FG_FLARED,
    HUDDESC_SEQ_Q,
    HUDDESC_SEQ_W,
    HUDDESC_SEQ_E,
    HUDDESC_SEQ_R,

    HUDDESC_COUNT
};

struct SHudDesc {
	std::string hudType;
    std::string path[HUDDESC_COUNT];
	sf::Vector2f position[GUI::UNIT_COUNT];
	sf::Vector2f size[GUI::UNIT_COUNT];
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

    int mSkillPoints, mAttrPoints;

    void NextLevel( bool ignoreSkillPoints, bool muteSound = false );

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

    void XPGained(float xp, bool ignoreSkillPoints = false, bool muteSound = false);
    float XPRequired();
    float XPPreviouslyRequired();
	inline float GetTotalXP(){ return mTotalXP; }
    int GetLevel();
	inline int GetSkillPoints() { return mSkillPoints; }

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
};

#endif /*CPLAYER_H_*/
