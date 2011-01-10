#ifndef __SITEMTEMPLATE_H__
#define __SITEMTEMPLATE_H__

#include <string>
#include "../Effects/CEffectHandle.h"

struct SItemTemplate
{
    unsigned itemLvl;
    CEffectHandle *effect;
    float dropRate;
    std::wstring name;
    std::string image;

    SItemTemplate(unsigned int level=~0) : itemLvl(level), effect(NULL), dropRate(0.01f) {} 
};

/// g³ównie dla STLa (definicja w CItemTemplate.cpp)
bool operator<(const SItemTemplate& a, const SItemTemplate& b);  
bool operator>(const SItemTemplate& a, const SItemTemplate& b);   
bool operator==(const SItemTemplate& a, const SItemTemplate& b);

#endif //__SITEMTEMPLATE_H__//
