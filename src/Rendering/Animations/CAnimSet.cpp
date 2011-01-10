#include "../../Utils/CXml.h"
#include "CAnimSet.h"
#include "CAnimationManager.h"
#include "SAnimation.h"

std::string CAnimSet::mAnimClassNames[] = { "default", "move","attack", "death", "strafe" };
int CAnimSet::mAnimClassNumber = sizeof(CAnimSet::mAnimClassNames) / sizeof(std::string);

CAnimSet::CAnimSet(){
    mAnims.resize(mAnimClassNumber);
}

int CAnimSet::ParseAnimClass(std::string &input){
    // najpierw domyslne
    for (int i = 0; i < mAnimClassNumber; i++) 
        if (mAnimClassNames[i]==input)
            return i;
    // a moze to liczba?
    int tmp;
    if (sscanf(input.c_str(),"%d",&tmp) == 1)
        return mAnimClassNumber+tmp;
    // nie to nie
    return -1;
}

void CAnimSet::SetAnimation(int ac, SAnimation *animPtr){
    if ((ac < 0) || (animPtr == NULL)) return;
    if ((unsigned int) ac >= mAnims.size()) mAnims.resize(ac+1);
    mAnims[ac] = CNameAnimPair(animPtr->name);
    mAnims[ac].SetAnim(animPtr);
}

void CAnimSet::SetAnimation(int ac, std::string &animName){
    if (ac < 0) return;
    if ((unsigned int) ac >= mAnims.size()) mAnims.resize(ac+1);
    mAnims[ac] = CNameAnimPair(animName);
}

std::string *CAnimSet::GetAnimName(int ac){
    if ((ac < 0) || ((unsigned int) ac >= mAnims.size()))
        return NULL;
    return &mAnims[ac].name;
}

SAnimation *CAnimSet::GetAnim(int ac){
    if ((ac < 0) || ((unsigned int) ac >= mAnims.size()))
        return NULL;
    if (!mAnims[ac].isReady){
        mAnims[ac].SetAnim(gAnimationManager.GetAnimation(mAnims[ac].name));
    }
    return mAnims[ac].anim;
}

void CAnimSet::Parse(CXml &xml,rapidxml::xml_node<> *root){
    std::string tmp;
    for (xml_node<> *node=xml.GetChild(root,"anim"); node; node=xml.GetSibl(node,"anim") ){
        tmp = xml.GetString(node,"type");
        int ac = ParseAnimClass(tmp);
        if (ac < 0) continue;
        tmp = xml.GetString(node,"name");
        SetAnimation(ac,tmp);
    }
}

