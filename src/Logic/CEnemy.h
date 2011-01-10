#ifndef CENEMY_H_
#define CENEMY_H_

#include <string>
#include "CActor.h"

class CActorAI;

class CEnemy: public CActor 
{
    friend class CPhysicalManager;

protected:
    CEnemy(const std::wstring& uniqueId);
    virtual ~CEnemy();

public:
    virtual void Update(float dt);
	virtual void Kill();

    inline CActorAI *GetAI() { return (CActorAI*)mController; }
};

#endif /*CENEMY_H_*/
