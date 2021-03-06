#ifndef _NPC_H_
#define _NPC_H_

#include <string>
#include "CActor.h"
#include "CDialogGraph.h"

class CActorAI;
class CInteractionTooltip;
class CPlayer;

class CNpc : public CActor
{
protected:
	CNpc( const std::wstring& uniqueId );
	virtual ~CNpc();

    // tox, 22 dec obrzydlistwo, jak sie tego pozbyc stad?
    int mInteractionTooltipId;
    // i tego? da sie?
    CInteractionTooltip * mInteractionTooltip;
    std::string mSellingItem;
    int mSellingPrice;
    bool mHasShop;

    static std::string mHints[];
    static unsigned int mHintsCount;

public:
	virtual void Update(float dt);

    void HandleCollisionWithPlayer(CPlayer * player);

    inline CActorAI* GetAI()				{ return (CActorAI*)mController; }
	inline CDialogGraph* GetDialogGraph()	{ return &mDialogGraph; }

    const std::string & GetSellingItem();
    void SetSellingItem(const std::string & sellingItem);

    inline int GetSellingPrice() { return mSellingPrice; }
    inline void SetSellingPrice(int price) { mSellingPrice = price; }

    inline bool HasShop() { return mHasShop; }
    inline void SetHasShop(bool shop) { mHasShop = shop; }

private:
	CDialogGraph mDialogGraph;
	friend class CPhysicalManager;
};

#endif

