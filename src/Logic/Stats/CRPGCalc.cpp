#include "CRPGCalc.h"
#include "../../Utils/CXml.h"
#include "EAspect.h"
#include <cmath>

template<> CRPGCalc* CSingleton<CRPGCalc>::msSingleton = 0;

using namespace rapidxml;

CRPGCalc::CRPGCalc(){
    CXml xml( "data/rpg.xml", "root" );
    xml_node<> *node;

    mUnknownLevel.requiredXP = 1 << 30; // mnogo ;)
    
    mAspectNormalisationFactor = xml.GetFloat(xml.GetChild(0,"normalisation-factor"));
    mLevelReferenceValue = xml.GetFloat(xml.GetChild(0,"level-reference-value"));

    // opisy kolejnych poziomow
    mLevels.push_back(SLevelDescr());
    for ( node=xml.GetChild(0,"level"); node; node=xml.GetSibl(node,"level") ){
        CRPGCalc::SLevelDescr ld;
        ld.requiredXP=xml.GetInt(xml.GetChild(node,"req-xp"));
        ld.attrPoints=xml.GetInt(xml.GetChild(node,"attr-points"));
        ld.skillPoints=xml.GetInt(xml.GetChild(node,"skill-points"));
        mLevels.push_back(ld);
    }
}

const CRPGCalc::SLevelDescr &CRPGCalc::GetLevelDescr(unsigned int level){
    if ((level == 0) || (level > mLevels.size()))
        return mUnknownLevel;
    else
        return mLevels[level-1];
}

float CRPGCalc::CalculateModifiedValue(float v, const CAspectBatch &aspects, int modifier){
    if ( !modifier )
        return v;

    float result = 1.0f;
    // poziom inaczej, aspekty inaczej - najpierw poziom umiejki
    if (aLevel & modifier){
        float lvl = aspects[aLevel];
        float tmp = (lvl + mLevelReferenceValue) / (mLevelReferenceValue + 1);
        result *= tmp * tmp;
    }
    // aspekty
    for (int i = 1; i < Aspects::aspectCount -1; i++){
        EAspect a = EAspect(1 << i);
        if (a & modifier){
            result *= aspects[a] / mAspectNormalisationFactor;
        }
    }
    // "inv"
	if (modifier & Aspects::invValue)
    {
        if ( result != 0.0f )
            return v / result;
    }
    // "log"
	if (modifier & Aspects::logValue){
		if (v <= 0 ){
			fprintf(stderr,"[WARNING] CRPGCalc::CalculateModifiedValue - cant calculate logarithm from non-positive value %f\n",v);
			return v;
		}
		return exp(log(v)*result);
	} else 
		return v * result;
}

float CRPGCalc::CalculateDamageAfterResistance(float dmg, DamageTypes::EDamageType dt, const CAspectBatch &aspects){
    EAspect a = DamageTypes::ResistanceAspect(dt);
    if (a == aNoAspect)
        return dmg;
    else {
        return dmg * exp(log(0.5f)*(aspects[a]/mAspectNormalisationFactor));
    }
}

