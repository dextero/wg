#ifndef __CLOOT_H__
#define __CLOOT_H__

#include "../../ResourceManager/IResource.h"
#include "../CPhysical.h"
#include "../Factory/SLootTemplate.h"

class CPlayer;
class CPhysicalManager;
class CItem;
class IOptionChooserHandler;

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
    const std::wstring & GetCommandOnTake(){ return commandOnTake; }
    void SetCommandOnTake(const std::wstring & value){ commandOnTake = value; }

    void SetItem(CItem * item) { mItem = item; }
    CItem * GetItem() { return mItem; }
protected:
    SLootTemplate *obj;  

private:
    std::wstring commandOnTake;

    IOptionChooserHandler * mOptionHandler;
    CItem * mItem;
};

#endif //__CLOOT_H__//
