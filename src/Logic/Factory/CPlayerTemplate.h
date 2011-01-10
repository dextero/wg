/* Wzorzec gracza */

#ifndef _PLAYER_TEMPLATE_H_
#define _PLAYER_TEMPLATE_H_

#include "CActorTemplate.h"
#include "../CPlayer.h"
#include <string>
#include <vector>

class CXml;
class CAbility;
class CAnimSet;
class CTriggerEffects;
class CAbilityTree;

class CPlayerTemplate : public CActorTemplate
{
public:
    CPlayerTemplate(): mTriggerEffects(NULL) {} 
	virtual ~CPlayerTemplate() { }

	inline float GetMaxRadius() { return mMaxRadius; }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	CPlayer* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return true; }

private:
    CTriggerEffects *mTriggerEffects;
    std::vector<CAbilityTree*> mAbilityTrees;
    std::vector<std::string> mAbilityTreeCodes;
    SHudDesc mHudDesc;
};

#endif // _PLAYER_TEMPLATE_H_

