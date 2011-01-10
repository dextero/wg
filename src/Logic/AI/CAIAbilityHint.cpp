#include "../../Utils/CXml.h"
#include "CAIAbilityHint.h"
#include "../Abilities/CAbility.h"
#include "../../Utils/MathsFunc.h"
#include "../../Utils/Directions.h"
#include "../CActor.h"
#include <cassert>

CAIAbilityHint::CAIAbilityHint(CAbility *abi):
mAbility(abi),
mDistanceToPlayer(0.f),
mPlayerAngle(0.f),
mHpLess(0.f),
mHpMore(0.f)
{
}

CAIAbilityHint::~CAIAbilityHint(){
}
    
void CAIAbilityHint::Load(CXml &xml, rapidxml::xml_node<> *node){
    xml_node<> *n = xml.GetChild(node,"player-distance");
    if (n){
        mDistanceToPlayer = xml.GetFloat(n);
    } else {
        mDistanceToPlayer = -1.0f;
    }
	n = xml.GetChild(node,"player-angle");
    if (n){
		mPlayerAngle = xml.GetFloat(n);
    } else {
        mPlayerAngle = -1.0f;
    }
    n = xml.GetChild(node, "hp-less");
    if (n){
        mHpLess = Maths::Clamp(xml.GetFloat(n));
    } else {
        mHpLess = 1.f;
    }
    n = xml.GetChild(node, "hp-more");
    if (n){
        mHpMore = Maths::Clamp(xml.GetFloat(n));
    } else {
        mHpMore = 0.f;
    }
}


bool CAIAbilityHint::UseAbility(CActor *attacker, CActor *player){
    assert(mAbility != NULL);
	int idx = attacker->GetAbilityPerformer().FindAbilityIndex(mAbility);
	ExecutionContextPtr context = attacker->GetAbilityPerformer().GetContext(idx);

    if (mAbility->abilityType == atMelee)
        if (!(mAbility->InMeleeRange(attacker,player,context)))
            return false;

    if (mDistanceToPlayer >= 0.0f)
        if (Maths::Length(attacker->GetPosition() - player->GetPosition()) > mDistanceToPlayer)
            return false;

	if (mPlayerAngle >= 0.0f){
		//warning C4244: 'argument' : conversion from 'int' to 'float', possible loss of data
		float rotation = (float)attacker->GetRotation();
		sf::Vector2f facing = RotationToVector(rotation);
		sf::Vector2f v = player->GetPosition() - attacker->GetPosition();
		if (Maths::Abs(Maths::AngleBetween(facing,v)) > mPlayerAngle)
			return false;
	}

    if (mHpLess < 1.f){
        if (mHpLess > attacker->GetHP() / attacker->GetStats()->GetBaseAspect(aMaxHP))
            return false;
    }
    if (mHpMore > 0.f){
        if (mHpMore < attacker->GetHP() / attacker->GetStats()->GetBaseAspect(aMaxHP))
            return false;
    }

    return true;
}

