#ifndef __EDAMAGETYPE_H__
#define __EDAMAGETYPE_H__

/**
 * Liosan, 16.10
 * Rodzaj obrazen - ogniowe, fizyczne itp; 
 * zwykle maja odpowiednik w aspektach - odpornosc.
 * domyslnym rodzajem obrazen jest dtPhysical
 */

#include <string>
#include "EAspect.h"

namespace DamageTypes{
    const std::wstring damageTypeNames[] = { L"physical", L"fire", L"ice",
        L"lightning", L"poison", L"vorpal" };

    const int damageTypeCount = sizeof(damageTypeNames)/sizeof(std::wstring);
    
    enum EDamageType { // dodajac nowy damagetype, nalezy poprawic damageTypeNames
        dtPhysical = 0,
        dtFire = 1,
        dtIce = 2,
        dtLightning = 3,
        dtPoison = 4,
        dtVorpal = 5 // brak odpornosci
    };

    EDamageType ParseDamageType(const std::wstring &s);

    std::wstring DamageTypeName(EDamageType dt);

    std::wstring AvailableDamageTypes();

    EAspect ResistanceAspect(EDamageType dt);
}


#endif /*__EDAMAGETYPE_H__*/
