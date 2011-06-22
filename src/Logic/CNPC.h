#ifndef _NPC_H_
#define _NPC_H_

#include <string>
#include "CActor.h"
#include "CDialogGraph.h"

class CActorAI;

class CNPC : public CActor
{
protected:
	CNPC( const std::wstring& uniqueId );
	virtual ~CNPC();

public:
	virtual void Update(float dt);

    inline CActorAI* GetAI()				{ return (CActorAI*)mController; }
	inline CDialogGraph* GetDialogGraph()	{ return &mDialogGraph; }

private:
	CDialogGraph mDialogGraph;
	friend class CPhysicalManager;
};

#endif

