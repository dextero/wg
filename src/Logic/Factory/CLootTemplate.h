// wzorzec przedmiotu-znajdzki
#ifndef __CLOOTTEMPLATE_H__
#define __CLOOTTEMPLATE_H__

#include "CPhysicalTemplate.h"
#include "SLootTemplate.h"
#include "../Loots/CLoot.h"

class CLootManager;

class CLootTemplate : public CPhysicalTemplate
{
    friend class CLootManager;
public:
    explicit CLootTemplate(unsigned lootLvl) : mLoot(lootLvl) {}
    CLootTemplate() {}
    ~CLootTemplate() {}

    virtual CTemplateParam * ReadParam(CXml & xml, xml_node<> * node, CTemplateParam * orig = NULL);
    virtual void Parametrise(CPhysical * loot, CTemplateParam * param);
    virtual void SerializeParam(std::ostream &out, CTemplateParam *param, int indent = 0);

    /* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	virtual void PrepareEditorPreviewer( CDisplayable* d );

    CLoot * Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return false; }
private:
    SLootTemplate mLoot;
    friend bool operator<(const CLootTemplate* a, const CLootTemplate &b);  
    friend bool operator>(const CLootTemplate* a, const CLootTemplate &b);   
    friend bool operator==(const CLootTemplate* a, const CLootTemplate &b);
};

/// glownie dla STLa
bool operator<(const CLootTemplate* a, const CLootTemplate &b);  
bool operator>(const CLootTemplate* a, const CLootTemplate &b);   
bool operator==(const CLootTemplate* a, const CLootTemplate &b);

#endif //__CLOOTTEMPLATE_H__//
