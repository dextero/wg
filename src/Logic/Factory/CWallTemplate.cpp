#include "CWallTemplate.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/ZIndexVals.h"
#include "../../Rendering/CDrawableManager.h"

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CWallTemplate::GetType()
{
	return "CWallTemplate";
}

bool CWallTemplate::Load(CXml &xml)
{
    if (!CPhysicalTemplate::Load(xml)) return false;
	xml_node<>*	node;

	if ( xml.GetString(0,"type") != "wall" )
		return false;

	for (node=xml.GetChild(0,"image"); node; node=xml.GetSibl(node,"image") )
		mImages.push_back( xml.GetString(node,"filename") );

    mColorRed = xml.GetFloat( "color", "r" , 1.0 );
    mColorGreen = xml.GetFloat( "color", "g", 1.0 );
    mColorBlue = xml.GetFloat( "color", "b", 1.0 );
    mColorAlpha = xml.GetFloat( "color", "a", 1.0 );

	mSize.x = xml.GetFloat( "size", "x", 1.0f );
	mSize.y = xml.GetFloat( "size", "y", 1.0f );

	return true;
}

void CWallTemplate::Drop()
{
	delete this;
}

void CWallTemplate::PrepareEditorPreviewer(CDisplayable* d)
{
	if ( mImages.size() > 0 )
	{
		d->SetStaticImage( mImages[0] );
		d->SetScale( mSize );
	}

	if ( d->GetZIndex() != Z_EDITOR_WALL )
	{
		gDrawableManager.UnregisterDrawable( d );
		gDrawableManager.RegisterDrawable( d, Z_EDITOR_WALL );
	}
}

CWall *CWallTemplate::Create(std::wstring id)
{
    CWall* wall = gPhysicalManager.CreateWall();
	wall->SetTemplate(this);
	wall->SetCategory( PHYSICAL_WALL );
	wall->SetBoundingRect( mSize );
	wall->SetColor( mColorRed, mColorGreen, mColorBlue, mColorAlpha );	

	if ( mImages.size() > 0 ) 
	{
		int imageNo = gRand.Rnd( 0, static_cast<unsigned>(mImages.size()) );
		wall->SetImage( mImages[imageNo] );        
	}

	if ( wall->GetDisplayable() )
		wall->GetDisplayable()->SetScale( mSize );

	return wall;
}

void CWallTemplate::SetImageFilename(const std::string& path)
{
    mImages.clear();
    if (path != "")
        mImages.push_back(path);
}
