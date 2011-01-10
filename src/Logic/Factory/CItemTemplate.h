// wzorzec przedmiotu
#ifndef __CITEMTEMPLATE_H__
#define __CITEMTEMPLATE_H__

#include "CPhysicalTemplate.h"
#include "SItemTemplate.h"
#include "../Items/CItem.h"

class CDropManager;

class CItemTemplate : public CPhysicalTemplate
{
    friend class CDropManager;
public:
    explicit CItemTemplate(unsigned itemLvl) : item(itemLvl) {}
    CItemTemplate() {}
    ~CItemTemplate() {}

    /* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	virtual void PrepareEditorPreviewer( CDisplayable* d );

    CItem* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return false; }
private:
    SItemTemplate item;
    friend bool operator<(const CItemTemplate* a, const CItemTemplate &b);  
    friend bool operator>(const CItemTemplate* a, const CItemTemplate &b);   
    friend bool operator==(const CItemTemplate* a, const CItemTemplate &b);
};

/// glownie dla STLa
bool operator<(const CItemTemplate* a, const CItemTemplate &b);  
bool operator>(const CItemTemplate* a, const CItemTemplate &b);   
bool operator==(const CItemTemplate* a, const CItemTemplate &b);

#endif //__CITEMTEMPLATE_H__//
