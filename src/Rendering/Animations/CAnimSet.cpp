#include "../../Utils/CXml.h"
#include "CAnimSet.h"
#include "CAnimationManager.h"
#include "SAnimation.h"

//std::string CAnimSet::mAnimClassNames[] = { "default", "move","attack", "death", "strafe" };
//int CAnimSet::mAnimClassNumber = sizeof(CAnimSet::mAnimClassNames) / sizeof(std::string);

std::string CAnimSet::acDefault = "default";
std::string CAnimSet::acMove = "move";
std::string CAnimSet::acAttack = "attack";
std::string CAnimSet::acDeath = "death";
std::string CAnimSet::acStrafe = "strafe";

CAnimSet::CAnimSet() {
}

void CAnimSet::SetAnimation(const std::string & ac, SAnimation *animPtr) {
    if (ac == "") return;
    if (animPtr == NULL) return;
    CNameAnimPair nap(animPtr->name);
    nap.SetAnim(animPtr);
    mAnims[ac] = nap;
}

void CAnimSet::SetAnimation(const std::string & ac, const std::string & animName){
    if (ac == "") return;
    mAnims[ac] = CNameAnimPair(animName);
}

std::string emptyString = "";
const std::string & CAnimSet::GetAnimName(const std::string & ac) {
    if (ac == "") return emptyString;
    NameAnimPairMap::iterator i = mAnims.find(ac);
    if (i == mAnims.end()) {
        return emptyString;
    }
    return i->second.name;
}

SAnimation * CAnimSet::GetAnim(const std::string & ac) {
    NameAnimPairMap::iterator i = mAnims.find(ac);
    if (i == mAnims.end()) {
        return NULL;
    }

    if (!i->second.isReady) {
        i->second.SetAnim(gAnimationManager.GetAnimation(i->second.name));
    }
    return i->second.anim;
}

void CAnimSet::Parse(CXml &xml,rapidxml::xml_node<> *root){
    std::string tmp;
    for (xml_node<> *node=xml.GetChild(root,"anim"); node; node=xml.GetSibl(node,"anim")) {
        std::string ac = xml.GetString(node,"type");
        if (ac == "") continue;
        tmp = xml.GetString(node,"name");
        SetAnimation(ac,tmp);
    }
}

