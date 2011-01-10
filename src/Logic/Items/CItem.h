#ifndef __CITEM_H__
#define __CITEM_H__

#include "../../ResourceManager/IResource.h"
#include "../CPhysical.h"
#include "../Factory/SItemTemplate.h"

class CActor;
class CPhysicalManager;

/// Klasa reprezentuje przedmiot (miksturka, powerup).
/// Na ogó³ przedmioty produkowane s¹ przez CDropManager.
class CItem : public CPhysical
{
    friend class CPhysicalManager;
    CItem(const std::wstring& uniqueId);
    virtual ~CItem() {}
public:
    virtual void Update( float dt ) {}
    virtual void Perform(CActor *actor) const;
    //--------------------
    const std::wstring &GetItemName() const { return obj->name; }    
    unsigned GetItemLvl() const { return obj->itemLvl; }
    float GetItemDropRate() const { return obj->dropRate; }

    void BindTemplate(SItemTemplate *tmpl){ obj = tmpl; }
    const std::wstring & GetCommandOnTake(){ return commandOnTake; }
    void SetCommandOnTake(const std::wstring & value){ commandOnTake = value; }
protected:
    SItemTemplate *obj;  

private:
    std::wstring commandOnTake;
};

#endif //__CITEM_H__//
