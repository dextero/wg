#ifndef __CRPGCALC_H__
#define __CRPGCALC_H__

#include <vector>
#include "CAspectBatch.h"
#include "EDamageType.h"
#include "../../Utils/CSingleton.h"

#define gRPGCalc CRPGCalc::GetSingleton()

/**
 * Liosan, 02.07.09
 * Singletonowy kalkulator roznych funkcji rpgowych (poziomy, modyfikatory umiejetnosci itp)
 *
 * Konfigurowany data/rpg.xml
 */

class CRPGCalc : public CSingleton<CRPGCalc>{
public:
    struct SLevelDescr{
        int requiredXP;
        int attrPoints;
        int skillPoints;

        SLevelDescr(): requiredXP(0), attrPoints(0), skillPoints(0){}
    };
private:
    float mLevelReferenceValue;
    float mAspectNormalisationFactor;

    SLevelDescr mUnknownLevel;
    std::vector<SLevelDescr> mLevels;
public:
    CRPGCalc();

    const SLevelDescr &GetLevelDescr(unsigned int level);

    float CalculateModifiedValue(float v, const CAspectBatch &aspects, int modifier);

    float CalculateDamageAfterResistance(float dmg, DamageTypes::EDamageType dt, const CAspectBatch &aspects);
};

#endif /*__CRPGCALC_H__*/
