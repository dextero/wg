/* Wzorzec sciany */

#ifndef _HOOK_TEMPLATE_H_
#define _HOOK_TEMPLATE_H_

#include "CPhysicalTemplate.h"
#include "../Effects/CEffectHandle.h"
#include "../MapObjects/CHook.h"
#include <string>
#include <vector>

class CXml;

class CHookTemplate : public CPhysicalTemplate
{
public:
    CHookTemplate(): effect(NULL) {}
	~CHookTemplate() { }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	virtual void PrepareEditorPreviewer( CDisplayable* d );

    CHook* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return true; }
private:
	CEffectHandle *effect;
};

#endif //_HOOK_TEMPLATE_H_

