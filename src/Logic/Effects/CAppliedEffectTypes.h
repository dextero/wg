#ifndef __CAPPLIEDEFFECTTYPEs_H__
#define __CAPPLIEDEFFECTTYPES_H__

#include "../../Utils/Memory/CRefPoolable.h"
#include "../../Utils/Memory/CPool.h"
#include "../../Utils/Memory/RefCntPtr.h"

class CAppliedEffect;
typedef Memory::CPool<CAppliedEffect> CAppliedEffectPool;
typedef Memory::RefCntPtr<CAppliedEffect> CAppliedEffectPtr;

class CEffectSource;
typedef Memory::RefCntPtr<CEffectSource> EffectSourcePtr;

#endif /*__CAPPLIEDEFFECTTYPES_H__*/
