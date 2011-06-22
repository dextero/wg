#ifndef __CEXECUTIONCONTEXT_H__
#define __CEXECUTIONCONTEXT_H__

#include <string>
#include "../../Utils/Memory/CRefPoolable.h"
#include "../../Utils/Memory/RefCntPtr.h"
#include "../../Utils/Memory/CSafePtr.h"
#include "../../Utils/Memory/CSafePtrable.h"
#include "../Stats/CAspectBatch.h"

class CPhysical;
class CActor;
struct SAbilityInstance;
class CAbility;
class CPinnedAbilityBatch;

/**
 * Liosan, 30.06
 * kontekst wykonania efektu - zawiera informacje o poziomie umiejetnosci aktora
 * oraz o poziomach 
 */
class CExecutionContext;

typedef Memory::RefCntPtr<CExecutionContext> ExecutionContextPtr;

class CExecutionContext : public Memory::CRefPoolable, public Memory::CSafePtrable<CActor> {
private:
    static int InstancesCount;
protected:
    friend class Memory::CPool<CExecutionContext>;

    CExecutionContext();
    ~CExecutionContext(){ InstancesCount--; }
    void Init(Memory::CSafePtr<CActor> caster, CPhysical *physical, SAbilityInstance *abi, CPinnedAbilityBatch *pinned);
    Memory::CSafePtr<CActor> mCaster;
	CPhysical *mPhysical;
	CPinnedAbilityBatch *mPinned;
public:
    static inline int GetInstanceCount(){ return InstancesCount; }
    CAspectBatch values;
	float abilityPower;

    bool StillValidFor(CPhysical *physical, SAbilityInstance *abi);
    
    static CExecutionContext *Alloc(Memory::CSafePtr<CActor> caster, CPhysical *physical, SAbilityInstance *abi, CPinnedAbilityBatch *pinned);
	static CExecutionContext *Alloc(Memory::CSafePtr<CActor> caster, CPhysical *physical, int abiLevel, CPinnedAbilityBatch *pinned);

	inline CPhysical *GetPhysical() { return mPhysical; }
    inline Memory::CSafePtr<CActor> GetCaster() { return mCaster; }
	inline CPinnedAbilityBatch *GetPinnedAbilityBatch() { return mPinned; }
};

#endif /*__CEXECUTIONCONTEXT_H__*/
