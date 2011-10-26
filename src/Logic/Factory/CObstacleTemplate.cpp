#include "CObstacleTemplate.h"
#include "CTemplateParam.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../MapObjects/CObstacle.h"
#include "../CPhysicalManager.h"
#include "../Stats/CStats.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/Animations/CAnimationManager.h"

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
    mTitle = xml.GetString("title");

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

CTemplateParam * CObstacleTemplate::ReadParam(CXml & xml, rapidxml::xml_node<> * node, CTemplateParam * orig) {
    if (orig == NULL) orig = new CTemplateParam();
    std::string title = xml.GetString(node, "title");
    if (!title.empty())
        orig->stringValues["title"] = title;
    return CPhysicalTemplate::ReadParam(xml,node,orig);
}

void CObstacleTemplate::Parametrise(CPhysical * phys, CTemplateParam * param) {
    CObstacle * obstacle = (CObstacle*)phys;
    if (param->stringValues.count("title") > 0) {
        obstacle->SetTitle(param->stringValues["title"]);
    }
    CPhysicalTemplate::Parametrise(obstacle, param);
}

void CObstacleTemplate::SerializeParam(std::ostream &out, CTemplateParam *param, int indent)
{
    // spit out 'title' ?
//    if (param != NULL){
//        if (param->intValues.count("level") > 0){
//            for (int i = 0; i < indent; ++i) out << "\t";
//            out << "<level>" << param->intValues["level"] << "</level>\n";
//        }
//    }
    CPhysicalTemplate::SerializeParam(out, param, indent);
}

