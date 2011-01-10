#ifndef __CBULLETEMITTER_H__
#define __CBULLETEMITTER_H__

/**
 * Liosan, 16.05: klasa pomocnicza, odpowiedzialna za strzelanie
 * pociskami (najczesciej w ramach umiejetnosci)
 */

#include <string>
#include <SFML/System/Vector2.hpp>
#include "../Effects/CExecutionContext.h"
#include "../Effects/CEffectSource.h"
#include "../Effects/CComputedValue.h"
#include "IDefferedLoadable.h"

class CPhysical;
class CBulletTemplate;
class CBullet;
class CEffectHandle;
class CTargetChooser;

class CBulletEmitter : public IDefferedLoadable{
private:
    CBullet *CreateBullet(ExecutionContextPtr context, EffectSourcePtr source);
	float GetSpeed(ExecutionContextPtr context);
public:
	enum ESpreadType{ stUnknown, stSingle, stEqual, stShotgun, stRandom };

    CBulletEmitter();
    ~CBulletEmitter();

    void Load(CXml &xml, rapidxml::xml_node<> *root = 0);
    void Shoot(CPhysical *phys,ExecutionContextPtr context = ExecutionContextPtr(NULL), EffectSourcePtr source = EffectSourcePtr(NULL));
    void Shoot(const sf::Vector2f &pos, int rot, float offset = 0.0f,ExecutionContextPtr context = ExecutionContextPtr(NULL), EffectSourcePtr source = EffectSourcePtr(NULL));

    CBulletTemplate *mBulletTemplate;
    CEffectHandle *mEffect;
	CComputedValue mTTL; // Time to live
    CComputedValue mSpeed;
    CComputedValue mAcceleration;
    CComputedValue mRange;
    CComputedValue mAngularWidth;
    CComputedValue mBulletCount;
	ESpreadType mSpreadType;
	CComputedValue mSpeedFuzziness;
	
	float mExtraOffset;

    CEffectHandle *mExplosion;

    int mBulletAngularSpeed;
    int mExplodeOnFade;

    CEffectHandle *mInFlightEffect;

    // maska kolidowania kategorii fizycznych:
    int mFilter; 

	// dla pociskow samonaprowadzajacych	
	CComputedValue mTimeForTracking;
	CComputedValue mTurningSpeed;
	CTargetChooser *mTargetChooser;
};

#endif /* __CBULLETEMITTER_H__ */


