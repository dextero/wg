#ifndef _AI_ABILITY_HINT_H_
#define _AI_ABILITY_HINT_H_

#include "CAIScheme.h"

class CXml;
namespace rapidxml{
    template<class Ch> class xml_node;
}
class CActor;
class CAbility;

class CAIAbilityHint
{
public:
    CAIAbilityHint(CAbility *abi);
    ~CAIAbilityHint();
    
    void Load(CXml &xml, rapidxml::xml_node<char> *node);

    bool UseAbility(CActor *attacker, CActor *player);
private:
    CAbility *mAbility;
    float mDistanceToPlayer;
	float mPlayerAngle;
    float mHpLess, mHpMore;
};

#endif /*_AI_ABILITY_HINT_H_*/
