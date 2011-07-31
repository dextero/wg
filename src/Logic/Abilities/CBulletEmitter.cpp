#include "../../Utils/CXml.h"
#include "CBulletEmitter.h"
#include "../Factory/CBulletTemplate.h"
#include "../CBullet.h"
#include "../Effects/CEffectManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../Utils/Maths.h"
#include "../../Utils/Directions.h"
#include "../../Utils/CRand.h"
#include <cstdio>
#include "../Effects/CEffectHandle.h"
#include "../PhysicalCategories.h"
#include "../Effects/CComputedValueFactory.h"
#include "CTargetChooser.h"

CBulletEmitter::CBulletEmitter():
    mBulletTemplate(0),
    mEffect(NULL),
    mSpreadType(stUnknown),
    mExtraOffset(0.f),
    mExplosion(NULL),
    mBulletAngularSpeed(0),
    mExplodeOnFade(0),
    mInFlightEffect(NULL),
    mFilter(0),
    mTargetChooser(NULL)
{
}

CBulletEmitter::~CBulletEmitter(){}

void CBulletEmitter::Load(CXml &xml,xml_node<> *root){
    mRange = CComputedValueFactory::Parse(xml,xml.GetChild(root,"range"),10000.0f);
	mTTL = CComputedValueFactory::Parse(xml,xml.GetChild(root,"ttl"),10000.0f);
	mSpeed = CComputedValueFactory::Parse(xml,xml.GetChild(root,"speed"),1.0f);
	mAcceleration = CComputedValueFactory::Parse(xml,xml.GetChild(root,"acceleration"));
	mBulletCount = CComputedValueFactory::Parse(xml,xml.GetChild(root,"count"),1.0f);
	mAngularWidth = CComputedValueFactory::Parse(xml,xml.GetChild(root,"width"),1.0f);
    mExplodeOnFade = xml.GetInt(xml.GetChild(root,"explode-on-fade"),"value",0);
    mExtraOffset = xml.GetFloat(xml.GetChild(root,"extra-offset"),"value",0.0);
    mBulletAngularSpeed = xml.GetInt(xml.GetChild(root,"bullet-angular-speed"),"value",0);
	mFilter = PHYSICAL_HOSTILES | PHYSICAL_WALL | PHYSICAL_DOOR | PHYSICAL_OBSTACLE; 
    if ( xml.GetChild(root,"filter") )
        mFilter = ParsePhysicalFilter( xml.GetString( xml.GetChild( root, "filter" ), "value" ) ); 
	else
		mFilter = PHYSICAL_HOSTILES | PHYSICAL_WALL | PHYSICAL_DOOR | PHYSICAL_OBSTACLE;

    mBulletTemplate = (CBulletTemplate*)gResourceManager.GetPhysicalTemplate(
        xml.GetString(xml.GetChild(root,"bullet"),"value")); 
    xml_node<> *node = xml.GetChild(root,"effect");
    if (node){
        mEffect = gEffectManager.LoadEffect(xml,node);
    } else
        mEffect = NULL;
    node = xml.GetChild(root,"explosion");
    if (node){
        mExplosion = gEffectManager.LoadEffect(xml,node);
    } else
        mExplosion = NULL;
    
    node = xml.GetChild(root,"flight-effect");
    if (node){
        mInFlightEffect = gEffectManager.LoadEffect(xml,node);
    } else
        mInFlightEffect = NULL;

	node = xml.GetChild(root,"target-chooser");
	if (node){
		mTargetChooser = new CTargetChooser();
		mTargetChooser->Load(xml,node);

		mTimeForTracking = CComputedValueFactory::Parse(xml,xml.GetChild(root,"time-for-tracking"),1.0f);
		mTurningSpeed = CComputedValueFactory::Parse(xml,xml.GetChild(root,"turning-speed"),100.0f);
	} else {
		mTargetChooser = NULL;
		mTimeForTracking = 0.0f;
		mTurningSpeed = 0.0f;
	}
	
	node = xml.GetChild(root,"spread-type");
	if (node){
		std::string st = xml.GetString(node,"value");
		if (st == "single") mSpreadType = stSingle;
		else if (st == "equal") mSpreadType = stEqual;
		else if (st == "shotgun") mSpreadType = stShotgun;
		else if (st == "random") mSpreadType = stRandom;
		else {
			fprintf(stderr,"Unable to parse bullet emitter spread type: %s\n",st.c_str());
			mSpreadType = stUnknown;
		}
	} else {
		mSpreadType = stUnknown;
	}
		
	node = xml.GetChild(root,"speed-fuzziness");
	if (node){
		mSpeedFuzziness = CComputedValueFactory::Parse(xml,node,0.0f);
	}
}

CBullet* CBulletEmitter::CreateBullet(ExecutionContextPtr context, EffectSourcePtr source){
	float ttl = mTTL.Evaluate(context);
	float range = mRange.Evaluate(context);
	float acceleration = mAcceleration.Evaluate(context);
    CBullet *bullet = mBulletTemplate->Create();
	bullet->SetSide(source->DeterminePhysicalSource()->GetSideAndCategory().side); // FIXME probably can explode // versus
    bullet->SetEffect(mEffect);
    bullet->SetExplosionEffect(mExplosion);
    bullet->range=range;
    bullet->TTL=ttl;
    bullet->rotSpeed = mBulletAngularSpeed;
    bullet->context = context;
    bullet->acceleration = acceleration;
    bullet->explodeOnFade = mExplodeOnFade;
    bullet->source = source;
    bullet->filter = mFilter;
	bullet->mTargetChooser = mTargetChooser;
	bullet->mTimeForTracking = mTimeForTracking.Evaluate(context);
	bullet->mTurningSpeed = mTurningSpeed.Evaluate(context);
    return bullet;
}

void CBulletEmitter::Shoot(CPhysical *phys,ExecutionContextPtr context, EffectSourcePtr source){
    Shoot(phys->GetPosition(),phys->GetRotation(),phys->GetCircleRadius(),context, source);
}

float CBulletEmitter::GetSpeed(ExecutionContextPtr context){
	float fuzz = mSpeedFuzziness.Evaluate(context);
	float fuzzmod = (1.0f + gRand.Rndf(0,fuzz) - fuzz / 2.0f);
	float speed = mSpeed.Evaluate(context) * fuzzmod;
	return speed;
}

void CBulletEmitter::Shoot(const sf::Vector2f &pos, int rot, float offset,ExecutionContextPtr context, EffectSourcePtr source){
	int angularWidth = (int)mAngularWidth.Evaluate(context);
	int bulletCount = (int)mBulletCount.Evaluate(context);
    if (mBulletTemplate) {
		offset += mExtraOffset;
        CBullet *bullet;
		// jeden pocisk na wprost
		if ((mSpreadType == stSingle) || ((mSpreadType == stUnknown) && (bulletCount == 1) && (angularWidth == 0))){
            bullet = CreateBullet(context, source);
            bullet->SetPosition( pos +
                RotationToVector((float)rot) * offset);
            bullet->SetVelocity(RotationToVector((float)rot)*GetSpeed(context) );
            bullet->SetRotation(rot);			
			if (mInFlightEffect)
				mInFlightEffect->Apply(bullet,source,context);
        // jeden pocisk w losowym kierunku
		} else if ((mSpreadType == stRandom) || ((mSpreadType == stUnknown) && (bulletCount == 1) && (angularWidth != 0))){
            bullet = CreateBullet(context, source);
            int newrot = (rot - angularWidth / 2 + gRand.Rnd(0,angularWidth) ) % 360;
            if (newrot < 0) newrot+=360;
            bullet->SetPosition( pos +
                RotationToVector((float)newrot) * offset);
            bullet->SetVelocity(RotationToVector((float)newrot)*GetSpeed(context) );
            bullet->SetRotation(newrot);
			if (mInFlightEffect)
				mInFlightEffect->Apply(bullet,source,context);
		// n pociskow losowo rozkladanych
		} else if (mSpreadType == stShotgun) {
            for (int i = 0; i < bulletCount; i++){
                bullet = CreateBullet(context, source);
				int newrot = (rot - angularWidth / 2 + gRand.Rnd(0,angularWidth) ) % 360;
                if (newrot < 0) newrot+=360;
                bullet->SetPosition( pos +
                    RotationToVector((float)newrot) * offset);
                bullet->SetVelocity(RotationToVector((float)newrot)*GetSpeed(context) );
                bullet->SetRotation(newrot);
				if (mInFlightEffect)
					mInFlightEffect->Apply(bullet,source,context);
            }
		// n pociskow rownomiernie rozkladanych
		} else { 
            int step = angularWidth / (bulletCount - 1);
            int newrot;
            for (int i = 0; i < bulletCount; i++){
                bullet = CreateBullet(context, source);
                newrot = (rot - angularWidth / 2 + i * step) % 360;
                if (newrot < 0) newrot+=360;
                bullet->SetPosition( pos +
                    RotationToVector((float)newrot) * offset);
                bullet->SetVelocity(RotationToVector((float)newrot)*GetSpeed(context) );
                bullet->SetRotation(newrot);
				if (mInFlightEffect)
					mInFlightEffect->Apply(bullet,source,context);
            }
        }
    }
}

