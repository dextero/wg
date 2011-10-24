#include "CDoorTemplate.h"

#include "../../Utils/CXml.h"
#include "../CPhysicalManager.h"
#include "../../Rendering/Animations/CAnimationManager.h"
#include "../../Rendering/CDisplayable.h"
#include "CTemplateParam.h"
#include "../Conditions/CCondition.h"
#include "../Effects/CEffectManager.h"
#include "../Effects/CEffectHandle.h"

using namespace rapidxml;

CDoorTemplate::CDoorTemplate() :
mClosedAnim(NULL),
mOpenedAnim(NULL)
{
    ;
}

CDoorTemplate::~CDoorTemplate(){
}

std::string CDoorTemplate::GetType()
{
	return "CDoorTemplate";
}

bool CDoorTemplate::Load(CXml &xml)
{
    if (!CPhysicalTemplate::Load(xml)) return false;
	xml_node<>*	node;

	if (xml.GetString(0,"type") != "door")
		return false;

    for (node=xml.GetChild(0,"anim"); node; node=xml.GetSibl(node,"anim")){
        std::string str = xml.GetString(node,"type");
        if (str == "opened")
            mOpenedAnim = gAnimationManager.GetAnimation(xml.GetString(node,"name"));
        else if (str == "closed")
            mClosedAnim = gAnimationManager.GetAnimation(xml.GetString(node,"name"));
    }

	return true;
}

void CDoorTemplate::Drop()
{
	delete this;
}

void CDoorTemplate::PrepareEditorPreviewer(CDisplayable* d)
{
	d->SetAnimation(mOpenedAnim);
}

CDoor *CDoorTemplate::Create(std::wstring id)
{
    CDoor* door = gPhysicalManager.CreateDoor(id);
    door->SetTemplate(this);
    door->SetAnimation(CDoor::dsOpened, mOpenedAnim);
    door->SetAnimation(CDoor::dsClosed, mClosedAnim);
    ((CPhysical*)(door))->SetAnimation(mClosedAnim);
    door->SetCategory(PHYSICAL_DOOR);
	return door;
}

/*
CTemplateParam *CDoorTemplate::ReadParam(CXml &xml, rapidxml::xml_node<> *node, CTemplateParam *p){
    if (p == NULL)
        p = new CTemplateParam();
    rapidxml::xml_node<> *n = xml.GetChild(node,"doorId");
    if (n != NULL){
        CCondition *cond = new CCondition();
        cond->Load(xml,n);
        p->values["cond"]=cond;
    }
    n = xml.GetChild(node,"effect-on-open");
    if (n != NULL){
        CEffectHandle *effect = gEffectManager.LoadEffect(xml,n);
        p->values["on-open"]=effect;
    }
    return CPhysicalTemplate::ReadParam(xml,node,p);
}

void CDoorTemplate::Parametrise(CPhysical *phys, CTemplateParam *param){
    CDoor *door = (CDoor*)phys;
    if (param->values.count("cond") > 0){
        CCondition *cond = (CCondition*)param->values["cond"];
        door->SetCondition(cond);
    }
    if (param->values.count("on-open") > 0){
        CEffectHandle *effect = (CEffectHandle*)param->values["on-open"];
        door->SetOnOpened(effect);
    }
    CPhysicalTemplate::Parametrise(phys,param);
}

void CDoorTemplate::SerializeParam(std::ostream &out, CTemplateParam *param, int indent){
    if (param != NULL){
        if (param->values["cond"] > 0){
            CCondition * cond = (CCondition*)param->values["cond"];
            cond->Serialize(out, indent + 1);
        }
        if (param->values["on-open"] > 0){
            CEffectHandle *effect = (CEffectHandle*)param->values["on-open"];
            for (int i = 0; i < indent; ++i, out << "\t");
            out << "<effect-on-open type=\"console\">\n";
            for (int i = 0; i < indent + 1; ++i, out << "\t");
            out << "<text>";
            effect->Serialize(out, indent + 2);
            out << "</text>\n";
            for (int i = 0; i < indent; ++i, out << "\t");
            out << "</effect-on-open>";
        }
    }
    CPhysicalTemplate::SerializeParam(out,param,indent);
}
*/

