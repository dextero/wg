#include "CAppliedEffect.h"
#include "../../Utils/Memory/CPool.h"
#include "CEffectSource.h"

int CAppliedEffect::InstancesCount = 0;

CAppliedEffectPool aePool;

CAppliedEffect::CAppliedEffect():
isValid(true),
durationLeft(0.0f),
reapplyLeft(0.0f),
reapplyTime(0.0f),
offset(0),
brokenFocus(false),
displayableEffectHandle(0){
    InstancesCount++;
};

CAppliedEffect *CAppliedEffect::Alloc(){
    return aePool.Alloc();
}

void CAppliedEffect::Invalidate() {
    isValid = false;
    meAsSource->Invalidate();
    if (source!=NULL)
        meAsSource->SetSecondary(source);
    meAsSource = NULL;
    source = NULL;
}
