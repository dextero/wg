/* Wzorzec pocisku */

#ifndef _BULLET_TEMPLATE_H_
#define _BULLET_TEMPLATE_H_

#include "CPhysicalTemplate.h"
#include "../CPhysicalManager.h"
#include "../CBullet.h"
#include <string>
#include <vector>

class CXml;

class CBulletTemplate : public CPhysicalTemplate
{
public:
    CBulletTemplate(): mMinDamage(0.f), mMaxDamage(0.f), mAnimation(NULL) {}
	~CBulletTemplate() { mImages.clear(); mEffects.clear(); mDestroyEffects.clear(); }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	CBullet* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return false; }

private:
	float mMinDamage,	mMaxDamage;

    SAnimation * mAnimation;
	std::vector<std::string> mImages;
	std::vector<std::string> mEffects;
	std::vector<std::string> mDestroyEffects;
};

#endif

