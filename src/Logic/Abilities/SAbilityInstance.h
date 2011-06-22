#ifndef __SABILITYINSTANCE_H__
#define __SABILITYINSTANCE_H__

/**
 * Liosan, 30.06.09:
 * klasa pomocnicza, reprezentuje poznana przez aktora umiejetnosc
 * w szczegolnosci zawiera informacje o poziomie umiejetnosci
 */

class CAbility;
struct SAbilityNode;

struct SAbilityInstance{
public:
    int level;
	bool triggerEnabled;
    //tox, 22 maja -> crimsonmode -> tutaj wsadzic trigger? mu mu...
    CAbility *ability;
    const SAbilityNode *abiNode;

    SAbilityInstance(CAbility *abi, int level = 0, const SAbilityNode *abilityNode = NULL): 
		level(level), triggerEnabled(true), ability(abi), abiNode(abilityNode){}
    ~SAbilityInstance(){}
};

#endif /* __CABILITYPERFORMER_H__ */

