#ifndef _NPC_H_
#define _NPC_H_

#include <string>
#include "CActor.h"
#include "CDialogGraph.h"

class CActorAI;
class CInteractionTooltip;
class CPlayer;

class CNPC : public CActor
{
protected:
	CNPC( const std::wstring& uniqueId );
	virtual ~CNPC();

    // tox, 22 dec obrzydlistwo, jak sie tego pozbyc stad?
    int mInteractionTooltipId;
    // i tego? da sie?
    CInteractionTooltip * mInteractionTooltip;

public:
	virtual void Update(float dt);

    void HandleCollisionWithPlayer(CPlayer * player);

    inline CActorAI* GetAI()				{ return (CActorAI*)mController; }
	inline CDialogGraph* GetDialogGraph()	{ return &mDialogGraph; }

private:
	CDialogGraph mDialogGraph;
	friend class CPhysicalManager;
};

#endif

