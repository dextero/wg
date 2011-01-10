/* Wzorzec potwora */

#ifndef _ENEMY_TEMPLATE_H_
#define _ENEMY_TEMPLATE_H_

#include "CActorTemplate.h"
#include <string>
#include <vector>

class CXml;
class CAbility;
class CAnimSet;

class CEnemyTemplate : public CActorTemplate
{
public:
	CEnemyTemplate() {} 
	virtual ~CEnemyTemplate() { mAi.clear(); }

	inline float GetMaxRadius() { return mMaxRadius; }
    inline const std::string& GetBestiaryName() { return mBestiaryName; }
    inline const std::string& GetBestiaryDesc() { return mBestiaryDesc; }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	CEnemy* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return false; }

private:
	std::vector<std::wstring> mAi;

    std::string mBestiaryName, mBestiaryDesc;
};

#endif // _ENEMY_TEMPLATE_H_

