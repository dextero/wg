/* Wzorzec gniazda */

#ifndef _LAIR_TEMPLATE_H_
#define _LAIR_TEMPLATE_H_

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include "CActorTemplate.h"
#include "../CLair.h"

class CLair;

class CLairTemplate : public CActorTemplate
{
public:
    CLairTemplate(): mMinSpawnLimit(0), mMaxSpawnLimit(0), mMinSpawnRate(0.f), mMaxSpawnRate(0.f), mGroupingHint(0) {}
	~CLairTemplate() { mMonsters.clear(); }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

    CLair* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return false; }

private:
	short mMinSpawnLimit,	mMaxSpawnLimit;
	float mMinSpawnRate,	mMaxSpawnRate;
    int mGroupingHint;

	sf::Vector2f mSpawnPosition;
    std::string mSpawnSound;
	std::vector<std::string> mMonsters;
    
public:
    virtual CTemplateParam *ReadParam(CXml &xml, xml_node<> *node, CTemplateParam *orig = NULL);
    virtual void Parametrise(CPhysical *door,CTemplateParam *param);
    virtual void SerializeParam(std::ostream &out, CTemplateParam *param, int indent);
};

#endif

