#ifndef CAISCHEME_H_
#define CAISCHEME_H_

#include <string>
#include "../../Utils/Memory/CSafePtr.h"
#include "CActorAI.h"
class CAISchemeManager;

class CAIScheme
{
public:	
    virtual ~CAIScheme() {};
	
	virtual void RegisterAI(Memory::CSafePtr<CActorAI> ai) {}
    virtual void UnregisterAI(Memory::CSafePtr<CActorAI> ai) {}

	virtual void Update(float dt) {}
    virtual void UpdateAI(CActorAIData *ai, float dt) = 0;
    virtual std::string GetName() const = 0;

	virtual void Init(CAISchemeManager &sm) = 0;
};

#endif /*CAISCHEME_H_*/
