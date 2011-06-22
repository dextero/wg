#include "CRegionDescriptor.h"
#include "../Logic/CPhysicalManager.h"
#include "../Logic/MapObjects/CRegion.h"
#include "../Utils/StringUtils.h"
#include "../Logic/Conditions/CCondition.h"
#include "../Logic/Effects/CEffectManager.h"
#include "../Logic/Effects/CEffectHandle.h"

#include <cstdio>

using namespace Map;

CRegionDescriptor::CRegionDescriptor(const std::string &_name, const sf::Vector2f _pos, float _scale):
name(_name), pos(_pos),cond(NULL), nextMap(""), nextMapRegion(""), rot(0.0f), scale(_scale), effect(NULL){
    static int nextId = 1;
    uniqueId = nextId++;

    if (_name == "")
        name = "region" + StringUtils::ToString(uniqueId);
}

CRegionDescriptor::~CRegionDescriptor(){
    fprintf(stderr,"destroying region descriptor %s\n",name.c_str());
}

CDynamicRegion *CRegionDescriptor::Create(){
    CDynamicRegion *r = gPhysicalManager.CreateRegion(StringUtils::ConvertToWString(name));
    r->SetPosition(pos);
    if (nextMap != ""){
        r->SetNextMap(&nextMap);
        if (nextMapRegion != "")
            r->SetNextMapRegion(&nextMapRegion);
    }
    r->SetCondition(cond);
    r->SetEffectOnEnter(effect);
    r->SetRotation(rot);
    r->SetScale(scale);
    r->SetDescriptorId(uniqueId);
    return r;
}

void CRegionDescriptor::Load(CXml &xml, xml_node<> *node){
    xml_node<> *n = xml.GetChild(node,"next-map");
    if (n) 
        nextMap = xml.GetString(n);
    n = xml.GetChild(node,"next-map-region");
    if (n)
        nextMapRegion = xml.GetString(n);
    n = xml.GetChild(node,"cond");
    if (n!=NULL){
        cond = new CCondition();
        cond->Load(xml,n);
    }
    n = xml.GetChild(node,"effect-on-enter");
    if (n != NULL){
        effect = gEffectManager.LoadEffect(xml,n);
    }
    rot = xml.GetFloat(node, "rot");
}

void CRegionDescriptor::Serialize(std::ostream &out, int indent){
    for (int i = 0; i < indent; ++i, out << "\t");
    out << "<region name=\"" << name << "\" x=\""
        << pos.x << "\" y=\"" << pos.y << "\" rot=\"" << rot << "\" scale=\"" << scale << "\">\n";
    if (nextMap != ""){
        for (int i = 0; i < indent + 1; ++i, out << "\t");
        out << "<next-map>" << nextMap << "</next-map>\n";
        for (int i = 0; i < indent + 1; ++i, out << "\t");
        out << "<next-map-region>" << nextMapRegion << "</next-map-region>\n";
    }
    if (cond != NULL){
        cond->Serialize(out, indent + 1);
    }
    for (int i = 0; i < indent; ++i, out << "\t");
    out << "</region>\n";
}

const std::string CRegionDescriptor::Serialize()
{
    std::stringstream ss;
    Serialize(ss, 0);
    return ss.str();
}
