#include "CObstacleTemplate.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/MathsFunc.h"
#include "../MapObjects/CObstacle.h"
#include "../CPhysicalManager.h"
#include "../Stats/CStats.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/Animations/CAnimationManager.h"
#include "../OptionChooser/CSimpleOptionHandler.h"

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CObstacleTemplate::GetType()
{
	return "CObstacleTemplate";
}

bool CObstacleTemplate::Load(CXml &xml)
{
    if (!CPhysicalTemplate::Load(xml)) return false;
	if ( xml.GetString(0,"type") != "obstacle" )
		return false;

    mMinStats = new CStats(NULL);
    mMinStats->Load(xml,"min");

    mMaxStats = new CStats(NULL);
    mMaxStats->Load(xml,"max");

    mDefaultAnim = gAnimationManager.GetAnimation(xml.GetString("def-anim"));
    mDeathAnim = gAnimationManager.GetAnimation(xml.GetString("die-anim"));

    mImage = xml.GetString("image");

    mColorRed = xml.GetFloat( "color", "r" , 1.0 );
    mColorGreen = xml.GetFloat( "color", "g", 1.0 );
    mColorBlue = xml.GetFloat( "color", "b", 1.0 );
    mColorAlpha = xml.GetFloat( "color", "a", 1.0 );

	return true;
}

void CObstacleTemplate::Drop()
{
	delete this;
}

void CObstacleTemplate::PrepareEditorPreviewer(CDisplayable* d)
{
	d->SetAnimation( mDefaultAnim );
}

CObstacle *CObstacleTemplate::Create(std::wstring id)
{
    CObstacle* obstacle = gPhysicalManager.CreateObstacle(id);
	
	obstacle->SetTemplate(this);
    obstacle->SetCategory(PHYSICAL_OBSTACLE);
    if (!mImage.empty()) {
        obstacle->SetImage(mImage);
    } else {
        obstacle->SetAnimation(mDefaultAnim);
    }
    obstacle->SetDeathAnim(mDeathAnim);
    obstacle->SetGenre(mGenre);
    if (mGenre == L"chest") {
        IOptionChooserHandler * handler = new CSimpleOptionHandler();
        obstacle->SetOptionHandler(handler);
        handler->mReferenceCounter++;
    }

    float f = gRand.Rndf( 0.0f, 1.0f );
    obstacle->GetStats()->LerpContent(mMinStats,mMaxStats,f);
    
    float scale = Lerp( mMinScale, mMaxScale, f );
    obstacle->SetBoundingCircle(Lerp( mMinRadius, mMaxRadius, f ));
       
    obstacle->SetColor( mColorRed, mColorGreen, mColorBlue, mColorAlpha );

    if ( obstacle->GetDisplayable() )
    {
		obstacle->GetDisplayable()->SetScale( scale, scale );
    }

	return obstacle;
}

