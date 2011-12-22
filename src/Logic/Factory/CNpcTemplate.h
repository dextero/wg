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
private:
	std::vector<std::string> mDialogs;
};

#endif

