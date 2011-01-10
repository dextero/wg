#include <cstdio>
#include "EDamageType.h"

using namespace DamageTypes;

std::wstring DamageTypes::AvailableDamageTypes(){
    std::wstring s = L"";
    for (int i = 1; i < damageTypeCount; i++){
        if (i != 1) s+=L',';
        s+=damageTypeNames[i];
    }
    return s;
}


EDamageType DamageTypes::ParseDamageType(const std::wstring &s){
    for (int i = 0; i < damageTypeCount; i++)
        if (damageTypeNames[i]==s){
            return EDamageType(i);
        }
    if (s != L"")
        fprintf(stderr,"Failed to parse damageType: %ls. Returning dtPhysical\n",s.c_str());
    return dtPhysical;
}

std::wstring DamageTypes::DamageTypeName(EDamageType dt){
    return damageTypeNames[(int)dt];
}

EAspect DamageTypes::ResistanceAspect(EDamageType dt){
    switch(dt){
        case dtPhysical: return aPhysResist;
        case dtFire: return aFireResist;
        case dtIce: return aIceResist;
        case dtLightning: return aLightningResist;
        case dtPoison: return aPoisonResist;
        case dtVorpal: return aNoAspect;
        default:
            fprintf(stderr,"[ERROR]unknown or unimplemented damage type: %d; returning noAspect as resistance aspect\n",(int)dt);
            return aNoAspect;
    }
}

