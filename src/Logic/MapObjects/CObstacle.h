#ifndef __COBSTACLE_H_
#define __COBSTACLE_H_

#include <string>
#include <vector>

#include "../CPhysical.h"
#include "../Stats/CStats.h"

class CPlayer;

struct SAnimation;

class IOptionChooserHandler;

class CObstacle: public CPhysical {
    friend class CPhysicalManager;
private:
    bool mDestroyed;
    float mDyingTime;
protected:
    CStats mStats;

    CObstacle(const std::wstring& uniqueId);
    virtual ~CObstacle();
    
    SAnimation *mDeathAnim;

    IOptionChooserHandler * mOptionHandler;
public:
    virtual CStats *GetStats();
    virtual void Update( float dt );
    virtual void Kill();

    void HandleCollisionWithPlayer(CPlayer * player);

	inline bool IsDestroyed(){ return mDestroyed; }

    inline void SetDeathAnim(SAnimation *anim){ mDeathAnim = anim; }

    inline void SetOptionHandler(IOptionChooserHandler * handler){ mOptionHandler = handler; }
};

#endif /*__COBSTACLE_H_*/
