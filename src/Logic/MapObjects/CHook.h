#ifndef CHOOK_H_
#define CHOOK_H_

#include <string>
#include <vector>

#include "../CPhysical.h"
#include "../Effects/CAppliedEffectContainer.h"

class CHook: public CPhysical {
    friend class CPhysicalManager;
protected:
    CHook(const std::wstring& uniqueId);
    virtual ~CHook();
	
    CAppliedEffectContainer mAppliedEffectContainer;
public:	
    virtual CAppliedEffectContainer *GetAppliedEffectContainer();
	virtual void Update( float dt );
};

#endif /*CWALL_H_*/
