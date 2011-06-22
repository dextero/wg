#include "CExecutionContext.h"
#include "../CPhysical.h"
#include "../CActor.h"
#include "../Abilities/SAbilityInstance.h"
#include "../Abilities/CAbility.h"
#include "../Stats/CStats.h"
#include "../../Utils/Memory/CPool.h"

int CExecutionContext::InstancesCount = 0;

Memory::CPool<CExecutionContext> xcPool;

CExecutionContext::CExecutionContext(): mCaster(Memory::CSafePtr<CActor>::NullPtr()), mPhysical(NULL), mPinned(NULL) {
    InstancesCount++;
}

void CExecutionContext::Init(Memory::CSafePtr<CActor> caster, CPhysical *physical, SAbilityInstance *abi, CPinnedAbilityBatch *pinned){
	mCaster = caster;
	mPhysical = physical;
	mPinned = pinned;
	CStats *stats = NULL;
	if (physical != NULL){
		stats = physical->GetStats();
	}
    if (stats != NULL){
        values = stats->GetAllCurrAspects();
    }
	abilityPower = 0;
	if (abi != NULL){
		int bonus = 0;
		if (caster != NULL) bonus = caster->ConsumeNextAbilityLevel();
		values.Set(aLevel,(float)abi->level + bonus);
	}
}

bool CExecutionContext::StillValidFor(CPhysical *physical, SAbilityInstance *abi){
    int stamp = -1;
    CStats *stats = physical->GetStats();
    if (stats != NULL){
        stamp = stats->GetAllCurrAspects().GetTimestamp();
    }
    if (stamp > values.GetTimestamp())
        return false;
    if (values[aLevel] != (float)abi->level )
        return false;
    return true;
}

CExecutionContext *CExecutionContext::Alloc(Memory::CSafePtr<CActor> caster, CPhysical *physical, SAbilityInstance *abi, CPinnedAbilityBatch *pinned){
    CExecutionContext *context = xcPool.Alloc();
    context->Init(caster, physical, abi, pinned);
    return context;
}

CExecutionContext *CExecutionContext::Alloc(Memory::CSafePtr<CActor> caster, CPhysical *physical, int abiLevel, CPinnedAbilityBatch *pinned){
    CExecutionContext *context = xcPool.Alloc();
    context->Init(caster, physical, NULL, pinned);
	//warning C4244: 'argument' : conversion from 'int' to 'float', possible loss of data
	context->values.Set(aLevel, (float)abiLevel);
    return context;
}
