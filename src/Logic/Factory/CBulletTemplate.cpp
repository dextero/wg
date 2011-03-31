#include "CBulletTemplate.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/Animations/CAnimationManager.h"
#include "../../Rendering/Animations/SAnimationState.h"
#include <iostream>

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CBulletTemplate::GetType()
{
	return "CBulletTemplate";
}

bool CBulletTemplate::Load(CXml &xml)
{
    if (!CPhysicalTemplate::Load(xml)) return false;
	xml_node<>*	node;

	if ( xml.GetString(0,"type") != "bullet" )
		return false;

	mMinDamage = xml.GetFloat( "damage", "min" );
	mMaxDamage = xml.GetFloat( "damage", "max" );

    node = xml.GetChild(0, "anim");
    if (node)
        mAnimation = gAnimationManager.GetAnimation(xml.GetString(node,"name"));

	for (node=xml.GetChild(0,"image"); node; node=xml.GetSibl(node,"image") )
		mImages.push_back( xml.GetString(node,"filename") );

	for (node=xml.GetChild(0,"effect"); node; node=xml.GetSibl(node,"effect") )
		mEffects.push_back( xml.GetString(node,"name") );

	for (node=xml.GetChild(0,"destroyEffect"); node; node=xml.GetSibl(node,"destroyEffect") )
		mDestroyEffects.push_back( xml.GetString(node,"name") );

	return true;
}

void CBulletTemplate::Drop()
{
	delete this;
}

CBullet* CBulletTemplate::Create(std::wstring id)
{
	CBullet* bullet = gPhysicalManager.CreateBullet();

	float random =	gRand.Rndf( 0.0f, 1.0f );
	float scale =	Lerp( mMinScale,	mMaxScale,	random );
	float radius =	Lerp( mMinRadius,	mMaxRadius,	random );

	bullet->SetCategory(PHYSICAL_BULLET);
	bullet->SetBoundingCircle(radius);

	if ( mImages.size() > 0 ) 
	{
		int imageNo = gRand.Rnd( 0, static_cast<unsigned>(mImages.size()) );
		bullet->SetImage( mImages[imageNo] );
	} else if (!mAnimation) {
		bullet->SetUseDisplayable(false);
	}

	if (mAnimation) {
		bullet->SetAnimation(mAnimation);
    }

	if ( bullet->GetDisplayable() )
		bullet->GetDisplayable()->SetScale( scale, scale );

	/* TODO:
	*  przypisywanie efektów cz¹steczkowych do pocisku
	*  ( czekam na system cz¹steczek z predefiniowanymi efektami identyfikowanymi po nazwie - stringu )
	*/

	return bullet;
}

