#ifndef _NPC_TEMPLATE_H_
#define _NPC_TEMPLATE_H_

#include "CActorTemplate.h"
#include <string>
#include <vector>

class CXml;

class CNPCTemplate : public CActorTemplate
{
public:
	CNPCTemplate() {} 
	virtual ~CNPCTemplate() { mDialogs.clear(); }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	CNPC* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return true; }
private:
	std::vector<std::string> mDialogs;
};

#endif

