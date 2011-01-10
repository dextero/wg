/* Wzorzec drzwi/bramy/blokady */

#ifndef _CDOOR_TEMPLATE_H_
#define _CDOOR_TEMPLATE_H_

#include "CPhysicalTemplate.h"
#include "../MapObjects/CDoor.h"
#include <string>

class CXml;
struct SAnimation;

class CDoorTemplate : public CPhysicalTemplate
{
public:
    CDoorTemplate();
    virtual ~CDoorTemplate();
	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	virtual void PrepareEditorPreviewer( CDisplayable* d );

	CDoor* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return true; }

protected:
    float mClosingTime, mOpeningTime;
    SAnimation *mClosedAnim, *mOpenAnim;
    SAnimation *mClosingAnim, *mOpeningAnim;
public:
    CTemplateParam *ReadParam(CXml &xml, xml_node<> *node,  CTemplateParam *orig = NULL);
    virtual void Parametrise(CPhysical *door,CTemplateParam *param);
    virtual void SerializeParam(std::ostream &out, CTemplateParam *param, int indent);
};

#endif // _CPHYSICAL_TEMPLATE_H_

