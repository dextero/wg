#ifndef __CLOOT_H__
#define __CLOOT_H__

#include "../../ResourceManager/IResource.h"
#include "../CPhysical.h"
#include "../Factory/SLootTemplate.h"

class CPlayer;
class CPhysicalManager;
class CItem;
class CInteractionTooltip;

/// Klasa reprezentuje przedmiot-znajdzke (miksturka, powerup) lezacy na ziemi.
/// Na ogol znajdzki produkowane sa przez CLootManager.
class CLoot : public CPhysical
{
    friend class CPhysicalManager;
    CLoot(const std::wstring& uniqueId);
    virtual ~CLoot();
public:
    virtual void Update( float dt ) {}
    virtual void HandleCollision(CPlayer * player);
    //--------------------
    const std::wstring &GetLootName() const { return obj->name; }    
    unsigned GetLootLvl() const { return obj->lootLvl; }
    float GetLootDropRate() const { return obj->dropRate; }

    void BindTemplate(SLootTemplate *tmpl){ obj = tmpl; }

    void SetAbility(const std::string & ability);

    void SetItem(CItem * item) { mItem = item; }
    CItem * GetItem() { return mItem; }

    void SetLevel(int level) { mLevel = level; }
    int GetLevel() { return mLevel; } 
protected:
    SLootTemplate *obj;  

    // tox, 23jan12 obrzydlistwo, jak sie tego pozbyc stad?
    int mInteractionTooltipId;
    CInteractionTooltip * mInteractionTooltip;
    //

private:
    CItem * mItem;
    int mLevel;
};

#endif //__CLOOT_H__//
