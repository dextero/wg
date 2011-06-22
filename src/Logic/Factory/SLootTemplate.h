#ifndef __SLOOTTEMPLATE_H__
#define __SLOOTTEMPLATE_H__

#include <string>

class CEffectHandle;

struct SLootTemplate
{
    unsigned lootLvl;
    CEffectHandle * effect;
    float dropRate;
    std::wstring name;
    std::string image;
    std::string ability;

    SLootTemplate(unsigned int level=~0) : lootLvl(level), effect(NULL), dropRate(0.01f) {} 
};

/// glownie dla STLa (definicja w CItemTemplate.cpp)
//bool operator<(const SLootTemplate& a, const SLootTemplate& b);  
//bool operator>(const SLootTemplate& a, const SLootTemplate& b);   
//bool operator==(const SLootTemplate& a, const SLootTemplate& b);

#endif //__SLOOTTEMPLATE_H__//
