#ifndef __SEFFECTNODE_H__
#define __SEFFECTNODE_H__

#include "EEffectType.h"
#include "CComputedValue.h"
#include "../Stats/EAspect.h"
#include "../../Rendering/Effects/EffectTypes.h"
#include "../Stats/EDamageType.h"

#include <string>
class CBulletEmitter;
class CTargetChooser;
class CSummoner;

struct SEffectNode {
	// TODO: wywalic
	std::string debugInfo;

    union{
        int iParam;
        EAspect aspect;
        EffectTypes::EffectType effectType;
        //EffectHandle grEffectHandle;
        DamageTypes::EDamageType damageType;
        std::string *sParam;
        std::wstring *wsParam;
        CBulletEmitter *bulletEmitter;
        CTargetChooser *targetChooser;
        CSummoner *summoner;
    };

	CComputedValue fValue;

    // offset w efekcie, ktory oznacza koniec tego efektu
    // ustalony tylko dla wezlow z effectType
	// obliczany w trakcie Compile
    int endOffset; 

    SEffectNode(): iParam(0), fValue(0.0f),endOffset(0){}
};

#endif /*__SEFFECTNODE_H__*/
