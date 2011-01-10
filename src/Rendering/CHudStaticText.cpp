#include "CHudStaticText.h"
#include "CCamera.h"
#include "../CGameOptions.h"
#include "../CGame.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CFont.h"
#include "../Utils/StringUtils.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <math.h>
#ifdef PLATFORM_MACOSX
#include <gl.h>
#else
#include <GL/gl.h>
#endif

CHudStaticText::CHudStaticText() :
    IDrawable(), 
	mSFString(NULL),
	mFontSize(1),
	mClipRectEnabled( false )
{
    memset(mClipRect, 0, 4 * sizeof(int));
    mSFString = new sf::String();
}

CHudStaticText::~CHudStaticText()
{
    delete mSFString;
}

void CHudStaticText::Draw( sf::RenderWindow* renderWindow ) {

    float zoom = gCamera.GetZoom();
	gCamera.SetZoom(1.0f); 

	sf::Vector2f cameraPosition = gGame.GetRenderWindow()->GetDefaultView().GetCenter();
	gGame.GetRenderWindow()->GetDefaultView().SetCenter( gGame.GetRenderWindow()->GetDefaultView().GetHalfSize() );

	if ( mClipRectEnabled )
	{
		glEnable( GL_SCISSOR_TEST );
		glScissor( mClipRect[0], mClipRect[1], mClipRect[2], mClipRect[3] );
	}

	renderWindow->Draw( *mSFString );

	glDisable( GL_SCISSOR_TEST );
    gCamera.SetZoom( zoom );
	gGame.GetRenderWindow()->GetDefaultView().SetCenter( cameraPosition );
}

sf::String* CHudStaticText::GetSFString()
{
    return mSFString;
}

void CHudStaticText::SetFont(const std::string &font, unsigned int size)
{
	mFont = font;
	mFontSize = size;

	if ( sf::Font* f = gResourceManager.GetFont( mFont + ':' + StringUtils::ToString(mFontSize) ) )
	{
		GetSFString()->SetFont( *f );
		GetSFString()->SetSize( (float) mFontSize );
	}
}

void CHudStaticText::SetFontSize(unsigned int size)
{
	if ( mFontSize != size )
	{
		mFontSize = size;

		if ( sf::Font* f = gResourceManager.GetFont( mFont + ':' + StringUtils::ToString(mFontSize) ) )
		{
			GetSFString()->SetFont( *f );
			GetSFString()->SetSize( (float) mFontSize );
		}
	}
}

void CHudStaticText::SetPosition( float x, float y )
{
	GetSFString()->SetPosition( floor(x), floor(y) );
}

void CHudStaticText::SetClipRect(int left, int top, int right, int bottom)
{
	mClipRectEnabled = true;
	mClipRect[0] = left;
	mClipRect[1] = gGameOptions.GetHeight() - bottom;
	mClipRect[2] = right - left;
	mClipRect[3] = bottom - top;
}

void CHudStaticText::SetClipRect(const sf::FloatRect &rect)
{
	SetClipRect( (int) rect.Left, (int) rect.Top, (int) rect.Right, (int) rect.Bottom );
}

