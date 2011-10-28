#ifndef __COBSTACLE_H_
#define __COBSTACLE_H_

#include <string>
#include <vector>

#include "../CPhysical.h"
#include "../Stats/CStats.h"

class CPlayer;

struct SAnimation;

class IOptionChooserHandler;
class InteractionHandler;
class CInteractionTooltip;

class CObstacle: public CPhysical {
    friend class CPhysicalManager;
private:
    bool mDestroyed;
    float mDyingTime;
protected:
    CStats mStats;
    std::string mTitle;

    CObstacle(const std::wstring& uniqueId);
    virtual ~CObstacle();
    
    SAnimation *mDeathAnim;

    IOptionChooserHandler * mOptionHandler;

    InteractionHandler * mInteractionHandler;
    CInteractionTooltip * mInteractionTooltip;

public:
    virtual CStats *GetStats();
    virtual void Update( float dt );
    virtual void Kill();

    void HandleCollisionWithPlayer(CPlayer * player);

	inline bool IsDestroyed(){ return mDestroyed; }

    inline void SetDeathAnim(SAnimation *anim){ mDeathAnim = anim; }
    
    void SetTitle(const std::string & title);
};

#endif /*__COBSTACLE_H_*/
