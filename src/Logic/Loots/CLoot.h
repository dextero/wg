#ifndef __CLOOT_H__
#define __CLOOT_H__

#include "../../ResourceManager/IResource.h"
#include "../CPhysical.h"
#include "../Factory/SLootTemplate.h"

class CActor;
class CPhysicalManager;

/// Klasa reprezentuje przedmiot-znajdzke (miksturka, powerup) lezacy na ziemi.
/// Na ogol znajdzki produkowane sa przez CLootManager.
class CLoot : public CPhysical
{
    friend class CPhysicalManager;
    CLoot(const std::wstring& uniqueId);
    virtual ~CLoot() {}
public:
    virtual void Update( float dt ) {}
    virtual void Perform(CActor *actor) const;
    //--------------------
    const std::wstring &GetLootName() const { return obj->name; }    
    unsigned GetLootLvl() const { return obj->lootLvl; }
    float GetLootDropRate() const { return obj->dropRate; }

    void BindTemplate(SLootTemplate *tmpl){ obj = tmpl; }
    const std::wstring & GetCommandOnTake(){ return commandOnTake; }
    void SetCommandOnTake(const std::wstring & value){ commandOnTake = value; }
protected:
    SLootTemplate *obj;  

private:
    std::wstring commandOnTake;
};

#endif //__CLOOT_H__//
