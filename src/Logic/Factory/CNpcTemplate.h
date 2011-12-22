#ifndef _NPC_TEMPLATE_H_
#define _NPC_TEMPLATE_H_

#include "CActorTemplate.h"
#include "../CNpc.h"
#include <string>
#include <vector>

class CXml;

class CNpcTemplate : public CActorTemplate
{
public:
	CNpcTemplate() {} 
	virtual ~CNpcTemplate() { mDialogs.clear(); }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	CNpc* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return true; }

    virtual CTemplateParam * ReadParam(CXml & xml, xml_node<> * node, CTemplateParam * orig = NULL);
    virtual void Parametrise(CPhysical * loot, CTemplateParam * param);
    virtual void SerializeParam(std::ostream &out, CTemplateParam *param, int indent = 0);

private:
	std::vector<std::string> mDialogs;
};

#endif

