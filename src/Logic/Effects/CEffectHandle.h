#ifndef __CEFFECTHANDLE_H__
#define __CEFFECTHANDLE_H__

#include <vector>
#include <string>
#include <cstdio>
#include <ostream>
#include <SFML/System/Vector2.hpp>

#include "../../ResourceManager/IResource.h"
#include "CEffectSource.h"
#include "CAppliedEffect.h"
#include "CExecutionContext.h"

/**
 * Liosan, 01.04 
 * Klasa stanowiaca uchwyt na efekt (nazwa i id)
 * Mozna go "wywolac" w uproszczony sposob, bez koniecznosci odwolywania sie
 * do CEffectManagera.
 * Radze wszystkim, ktorzy efektow chca uzywac bez wiekszych kombinacji,
 * zadowoleniem sie tym co znajduje sie ponizej :) 
 * Bo glebiej zaczynaja sie wieksze kombinacje, oj tak.
 */

class CPhysical;
class CActor;
struct SEffectDamage;

class CEffectHandle: public IResource {
    friend class CEffectManager;
private:
    unsigned int offset;
    virtual bool Load(std::map<std::string,std::string>& argv);
public:
    unsigned int GetOffset(){
        return offset;
    }

    CEffectHandle(int offset){
        this->offset = offset;
    }

    virtual ~CEffectHandle(){}

    void Apply(CPhysical *physical, EffectSourcePtr source = EffectSourcePtr(NULL), ExecutionContextPtr context = ExecutionContextPtr(NULL));
    void ApplyPermanent(CPhysical *physical, EffectSourcePtr source = EffectSourcePtr(NULL), ExecutionContextPtr context = ExecutionContextPtr(NULL));

    void PerformAt(const sf::Vector2f &pos, EffectSourcePtr source = EffectSourcePtr(NULL), ExecutionContextPtr context = ExecutionContextPtr(NULL));

    virtual void Drop();

	virtual std::string GetType();

    void Serialize(std::ostream &out, int indent);
	const std::string SerializeConsoleFriendly();
};

#endif /*__CEFFECTHANDLE_H__*/
